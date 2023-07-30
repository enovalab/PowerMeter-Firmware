#ifdef ESP32

#include "Config.h"
#include "Log/Log.h"
#include "ExceptionTrace/ExceptionTrace.h"
#include "Globals/Globals.h"
#include <Arduino.h>
#include <WiFi.h>
#include <fstream>


void Config::configureLogger()
{
    try
    {
        Logger[Level::Info] << "Configuring Logger..." << std::endl;
        json loggerConfigJson = Globals::LoggerConfigURI.deserialize();

        Serial.begin(loggerConfigJson.at("baudRate"));
        Logger.setLevel(Log::getLevelByName(loggerConfigJson.at("level")));
        Logger.setShowLevel(loggerConfigJson.at("showLevel"));
        const std::string& logFilePath = loggerConfigJson.at("logFile");

        static std::ofstream logFile;
        if (loggerConfigJson.at("fileMode"))
        {
            logFile.open(logFilePath);
            Logger.setOutputStream(&logFile);
        }

        Logger[Level::Info] << "Logger configured sucessfully." << std::endl;
    }
    catch (...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to configure 'Logger' from \"" << Globals::LoggerConfigURI << '"';
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void Config::configureACPowerMeter()
{
    Logger[Level::Info] << "Configuring 'ACPowerMeter'..." << std::endl;
    try
    {
        json powerMeterConfigJson = Globals::MeasuringConfigURI.deserialize();

        uint16_t pinU = powerMeterConfigJson.at("/pins/voltage"_json_pointer);
        uint16_t pinI = powerMeterConfigJson.at("/pins/current"_json_pointer);
        Globals::PowerMeter.setPins(pinU, pinI);

        float calU = powerMeterConfigJson.at("/calibration/voltage"_json_pointer);
        float calI = powerMeterConfigJson.at("/calibration/current"_json_pointer);
        float calPhase = powerMeterConfigJson.at("/calibration/phase"_json_pointer);
        Globals::PowerMeter.calibrate(calU, calI, calPhase);

        Logger[Level::Info] << "Measuring unit configured sucessfully." << std::endl;
    }
    catch (...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to configure 'ACPowerMeter' from \"" << Globals::MeasuringConfigURI << '"';
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void Config::configureRelay()
{
    try
    {
        Logger[Level::Info] << "Configuring relay..." << std::endl;

        json relayConfigJson = Globals::RelayConfigURI.deserialize();
        uint16_t pin = relayConfigJson.at("pin");
        bool state = Globals::RelayStateURI.deserialize();
        pinMode(pin, OUTPUT);
        digitalWrite(pin, state);

        Logger[Level::Info] << "Relay configured sucessfully." << std::endl;
    }
    catch (...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to configure Relay from \"" << Globals::RelayConfigURI << '"';
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void Config::configureTrackers()
{
    try
    {
        Logger[Level::Info] << "Configuring trackers..." << std::endl;

        json jsonTrackers = Globals::TrackersConfigURI.deserialize();
        for(const auto& jsonTracker : jsonTrackers.items())
        {
            std::string key = jsonTracker.key();
            std::stringstream trackerDirectory;
            trackerDirectory << "/Trackers/" << key << '/';

            Globals::Trackers.emplace(key, Tracker(
                jsonTracker.value().at("title"),
                jsonTracker.value().at("duration_s"),
                jsonTracker.value().at("sampleCount"),
                Globals::RTC,
                JsonURI(trackerDirectory.str() + "data.json"),
                JsonURI(trackerDirectory.str() + "timestamps.json#/lastInput"),
                JsonURI(trackerDirectory.str() + "timestamps.json#/lastSample"),
                AverageAccumulator(JsonURI(trackerDirectory.str() + "accumulator.json"))
            ));
        }

        Logger[Level::Info] << "Trackers configured sucessfully." << std::endl;
    }
    catch (...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to configure trackers from \"" << Globals::TrackersConfigURI << '"';
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


bool Config::configureWiFiStationary()
{
    Logger[Level::Info] << "Configuring WiFi in stationary mode..." << std::endl;
    JsonURI staConfigURI = Globals::WiFiConfigURI / "/sta"_json_pointer;
    try
    {
        json staConfigJson = staConfigURI.deserialize();

        const std::string& ssid = staConfigJson.at("ssid");
        const std::string& password = staConfigJson.at("password");
        const std::string& staticIP = staConfigJson.at("staticIP");
        const std::string& gateway = staConfigJson.at("gateway");
        const std::string& subnet = staConfigJson.at("subnet");

        auto parseIP = [](const std::string& address)
        {
            IPAddress ip;
            if (!ip.fromString(address.c_str()))
                Logger[Level::Warning] << SOURCE_LOCATION << "Could not convert \"" << address << "\" to 'IPAdress'" << std::endl;
            return ip;
        };
        
        WiFi.disconnect(true);
        WiFi.config(parseIP(staticIP), parseIP(gateway), parseIP(subnet));
        WiFi.begin(ssid.c_str(), password.c_str());
        WiFi.setSleep(false);

        if(WiFi.waitForConnectResult(3000) == WL_CONNECTED)
            return true;
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to configure WiFi in Stationary Mode from \"" << staConfigURI << '"';
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
    return false;
}


bool Config::configureWifiAccesspoint()
{
    Logger[Level::Info] << "Configuring WiFi as accesspoint..." << std::endl;
    JsonURI apConfigURI = Globals::WiFiConfigURI / "/ap"_json_pointer;
    try
    {
        json apConfigJson = apConfigURI.deserialize();
        const std::string& ssid = apConfigJson.at("ssid");
        const std::string& password = apConfigJson.at("password");

        return WiFi.softAP(ssid.c_str(), password.c_str());
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to configure WiFi as accesspoint from \"" << apConfigURI << '"';
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}

void Config::configureWiFi()
{
    Logger[Level::Info] << "Configuring WiFi..." << std::endl;

    try
    {
        json wifiConfigJson = Globals::WiFiConfigURI.deserialize();

        WiFi.mode(WiFiMode_t::WIFI_MODE_STA);
        if(configureWiFiStationary())
        {
            Logger[Level::Info] 
                << "WiFi connected sucessfully to '"
                << wifiConfigJson.at("sta").at("ssid")
                << "'. " 
                << "IP: http://" << WiFi.localIP().toString().c_str()
                << std::endl;
            return;
        }

        Logger[Level::Info] 
            << "Couldn't connect to" 
            << wifiConfigJson.at("sta").at("ssid")
            << ". Setting up Acesspoint..."
            << std::endl;

        WiFi.mode(WiFiMode_t::WIFI_MODE_AP);
        if(configureWifiAccesspoint())
        {
            Logger[Level::Info] 
                << "WiFi configured sucessfully in Acesspoint Mode. Network Name: "
                << wifiConfigJson.at("ap").at("ssid")
                << " IP: http://" << WiFi.softAPIP().toString().c_str()
                << std::endl;
            return;
        }

        throw std::runtime_error("Failed to configure WiFi");        
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to configure WiFi from \"" << Globals::WiFiConfigURI << '"';
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}

#endif