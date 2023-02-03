#ifdef ESP32

#include "System/PowerMeter.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"
#include "Measuring/ACPowerMeter.h"
#include "Time/DS3231.h"
#include "Data/Tracker.h"
#include "Connectivity/RestAPI.h"

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <fstream>

using namespace System;

namespace
{
    Measuring::ACPowerMeter powerMeter(33, 32);
    Measuring::ACPower power(0, 0, 0);
    AsyncWebServer server(80);
    Time::DS3231 rtc;
    Data::Tracker tracker(rtc);

    void configureLogger(const Data::JsonURI& configURI)
    {
        try
        {
            json configJson = configURI.deserialize();

            Serial.begin(configJson.at("baudRate"));
            Diagnostics::Logger.setLevel(Diagnostics::Log::getLevelByName(configJson.at("level")));
            Diagnostics::Logger.setShowLevel(configJson.at("showLevel"));

            if(configJson.at("fileMode"))
            {
                static std::ofstream logFile(configJson.at("logFile"));
                Diagnostics::Logger.setOutputStream(&logFile);
            }
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Diagnostics::Logger' from \"" << configURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureACPowerMeter(const Data::JsonURI& configURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring measuring unit..." << std::endl;
        try
        {
            json configJson = configURI.deserialize();
            powerMeter = Measuring::ACPowerMeter(
                configJson.at("/pins/voltage"_json_pointer),
                configJson.at("/pins/current"_json_pointer)
            );

            powerMeter.calibrate(
                configJson.at("/calibration/voltage"_json_pointer),
                configJson.at("/calibration/current"_json_pointer),
                configJson.at("/calibration/phase"_json_pointer)
            );
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Measuring::PowerMeter' from \"" << configURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureRelay(const Data::JsonURI& configURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring Relay..." << std::endl;
        try
        {
            json configJson = configURI.deserialize();
            int pin = configJson.at("pin");
            bool state = configJson.at("state");
            pinMode(pin, OUTPUT);
            digitalWrite(pin, state);
            Diagnostics::Logger[Level::Info] << "Relay configured. Pin " << pin << " set to " << std::boolalpha << state << std::endl;
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure Relay from \"" << configURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureTracker(const Data::JsonURI& configURI)
    {
        try
        {
            json jsonTrackingSpans = configURI.deserialize();
            std::vector<Data::TrackingSpan> trackingSpans;
            for(const auto& jsonTrackingSpan : jsonTrackingSpans)
            {
                trackingSpans.push_back(Data::TrackingSpan(
                    Data::JsonURI(jsonTrackingSpan.at("targetURI")),
                    Data::JsonURI(jsonTrackingSpan.at("lastSampleURI")),
                    jsonTrackingSpan.at("timeSpanSeconds"),
                    jsonTrackingSpan.at("numSamplesPerSpan"),
                    Data::JsonURI(jsonTrackingSpan.value("averageURI", ""))
                ));
            }
            tracker.setTrackingSpans(trackingSpans);
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Data::Tracker' from \"" << configURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
        }
    }


    bool connectWifiStationary(const Data::JsonURI& configURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring WiFi in stationary mode..." << std::endl;
        try
        {
            json configJson = configURI.deserialize();

            const std::string& ssid = configJson.at("ssid");
            const std::string& password = configJson.at("password");
            const std::string& staticIP = configJson.at("staticIP");
            const std::string& gateway = configJson.at("gateway");
            const std::string& subnet = configJson.at("subnet");
            const std::string& primaryDNS = configJson.at("primaryDNS");
            const std::string& secondaryDNS = configJson.at("secondaryDNS");

            auto parseIP = [](const std::string& address) 
            {
                IPAddress ip;
                if(!ip.fromString(address.c_str()))
                    Diagnostics::Logger[Level::Warning] << SOURCE_LOCATION << "Could not convert \"" << address << "\" to 'IPAdress'" << std::endl;
                return ip;
            };
            
            WiFi.mode(WIFI_STA);
            WiFi.config(parseIP(staticIP), parseIP(gateway), parseIP(subnet), parseIP(primaryDNS), parseIP(secondaryDNS));
            WiFi.begin(ssid.c_str(), password.c_str());
            delay(500);
            if(WiFi.status() == WL_CONNECTED)
            {
                Diagnostics::Logger[Level::Info] << "Sucessfully connected to " << ssid  << " IP: http://" << WiFi.localIP().toString().c_str() << std::endl;
                return true;
            }
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure WiFi as Stationary from \"" << configURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
        return false;
    }


    void configureWifiAccesspoint(const Data::JsonURI& configURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring WiFi as sccesspoint..." << std::endl;
        try
        {
            json configJson = configURI.deserialize();
            const std::string& ssid = configJson.at("ssid");
            const std::string& password = configJson.at("password");

            WiFi.mode(WIFI_AP);
            WiFi.softAP(ssid.c_str(), password.c_str());
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure WiFi as Accesspoint from \"" << configURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureWifi(const Data::JsonURI& configURI)
    {
        if(!connectWifiStationary(configURI / "/sta"_json_pointer))
            configureWifiAccesspoint(configURI / "/ap"_json_pointer);
    }


    void addAPIHandlers()
    {
        Diagnostics::Logger[Level::Info] << "Adding API Handlers..." << std::endl;
        Connectivity::RestAPI api(&server, "/api");

        api.handle(Connectivity::HTTP::Method::Get, "/power", [](json){
            json data;
            data["voltage"] = power.getVoltageRms();
            data["current"] = power.getCurrentRms();
            data["active"] = power.getActivePower();
            data["apparent"] = power.getApparentPower();
            data["reactive"] = power.getReactivePower();
            data["powerFactor"] = power.getPowerFactor();
            return Connectivity::RestAPI::JsonResponse(data);
        });

        auto getFromJsonURI = [](const json& data, const std::string& jsonURI) {
            return Connectivity::RestAPI::JsonResponse(Data::JsonURI(jsonURI).deserialize());
        };

        using namespace std::placeholders;

        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last60min", std::bind(getFromJsonURI, _1, POWERMETER_TRACKER_LAST60MIN_URI));
        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last24h", std::bind(getFromJsonURI, _1, POWERMETER_TRACKER_LAST24H_URI));
        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last7d", std::bind(getFromJsonURI, _1, POWERMETER_TRACKER_LAST7D_URI));
        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last30d", std::bind(getFromJsonURI, _1, POWERMETER_TRACKER_LAST30D_URI));
        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last12m", std::bind(getFromJsonURI, _1, POWERMETER_TRACKER_LAST12M_URI));
        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last10a", std::bind(getFromJsonURI, _1, POWERMETER_TRACKER_LAST10A_URI));
    }
}

void PowerMeter::init() noexcept
{
    try
    {
        Serial.begin(115200);
        LittleFS.begin(POWERMETER_FORMAT_FS_ON_FAIL);

        configureLogger(Data::JsonURI(POWERMETER_LOGGER_CONFIG_URI));
        Diagnostics::Logger[Level::Info] << "Booting..." << std::endl;

        configureWifi(Data::JsonURI(POWERMETER_WIFI_CONFIG_URI));
        configureACPowerMeter(Data::JsonURI(POWERMETER_ACPOWERMETER_CONFIG_URI));
        configureRelay(Data::JsonURI(POWERMETER_RELAY_CONFIG_URI));
        configureTracker(Data::JsonURI(POWERMETER_TRACKER_CONFIG_URI));
        addAPIHandlers();
        server.serveStatic("/", LittleFS, "/app")
            .setDefaultFile("index.html");

        server.begin();
        
        rtc.begin();
    }
    catch(...)
    {
        Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << Diagnostics::ExceptionTrace::what() << std::endl;
    }
    Diagnostics::Logger[Level::Info] << "Boot finished. Running..." << std::endl;
}


void PowerMeter::run() noexcept
{
    try
    {
        power = powerMeter.measure();
        tracker.track(power.getActivePower());
    }
    catch(...)
    {
        Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << Diagnostics::ExceptionTrace::what() << std::endl;
    }
}

#endif