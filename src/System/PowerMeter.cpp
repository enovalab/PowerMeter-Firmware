#ifdef ESP32

#include "System/PowerMeter.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"
#include "Measuring/ACPowerMeter.h"
#include "Time/DS3231.h"
#include "Data/Tracker.h"
#include "Connectivity/RestAPI.h"

#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <LittleFS.h>
#include <fstream>

using namespace System;

namespace
{
    Measuring::ACPowerMeter powerMeter(33, 32);
    Measuring::ACPower power(0, 0, 0);
    AsyncWebServer server(80);
    Connectivity::RestAPI api(&server, "/api");
    Time::DS3231 rtc;
    Data::Tracker tracker(rtc);

    void configureLogger(const Data::JsonURI& loggerConfigURI)
    {
        try
        {
            json loggerConfigJson = loggerConfigURI.deserialize();

            Serial.begin(loggerConfigJson.at("baudRate"));
            Diagnostics::Logger.setLevel(Diagnostics::Log::getLevelByName(loggerConfigJson.at("level")));
            Diagnostics::Logger.setShowLevel(loggerConfigJson.at("showLevel"));

            if(loggerConfigJson.at("fileMode"))
            {
                static std::ofstream logFile(loggerConfigJson.at("logFile"));
                Diagnostics::Logger.setOutputStream(&logFile);
            }
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Diagnostics::Logger' from \"" << loggerConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureACPowerMeter(const Data::JsonURI& powerMeterConfigURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring measuring unit..." << std::endl;
        try
        {
            json powerMeterConfigJson = powerMeterConfigURI.deserialize();

            const uint16_t& pinU = powerMeterConfigJson.at("/pins/voltage"_json_pointer); 
            const uint16_t& pinI = powerMeterConfigJson.at("/pins/current"_json_pointer);

            powerMeter = Measuring::ACPowerMeter(pinU, pinI);

            const float& calU = powerMeterConfigJson.at("/calibration/voltage"_json_pointer);
            const float& calI = powerMeterConfigJson.at("/calibration/current"_json_pointer);
            const float& calPhase = powerMeterConfigJson.at("/calibration/phase"_json_pointer);

            powerMeter.calibrate(calU, calI, calPhase);
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Measuring::PowerMeter' from \"" << powerMeterConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureRelay(const Data::JsonURI& relayConfigURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring Relay..." << std::endl;
        try
        {
            json relayConfigJson = relayConfigURI.deserialize();
            const uint16_t& pin = relayConfigJson.at("pin");;
            bool state = relayConfigJson.at("state");
            pinMode(pin, OUTPUT);
            digitalWrite(pin, state);
            Diagnostics::Logger[Level::Info] << "Relay configured. Pin " << pin << " set to " << std::boolalpha << state << std::endl;
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure Relay from \"" << relayConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureTracker(const Data::JsonURI& trackerConfigURI)
    {
        try
        {
            json jsonTrackingSpans = trackerConfigURI.deserialize();
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
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Data::Tracker' from \"" << trackerConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
        }
    }


    bool connectWifiStationary(const Data::JsonURI& staConfigURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring WiFi in stationary mode..." << std::endl;
        try
        {
            json staConfigJson = staConfigURI.deserialize();

            const std::string& ssid = staConfigJson.at("ssid");
            const std::string& password = staConfigJson.at("password");
            const std::string& staticIP = staConfigJson.at("staticIP");
            const std::string& gateway = staConfigJson.at("gateway");
            const std::string& subnet = staConfigJson.at("subnet");
            const std::string& primaryDNS = staConfigJson.at("primaryDNS");
            const std::string& secondaryDNS = staConfigJson.at("secondaryDNS");

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
            if(WiFi.waitForConnectResult() == WL_CONNECTED)
            {
                Diagnostics::Logger[Level::Info] << "Sucessfully connected to " << ssid  << " IP: http://" << WiFi.localIP().toString().c_str() << std::endl;
                return true;
            }
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure WiFi in Stationary Mode from \"" << staConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
        return false;
    }


    void configureWifiAccesspoint(const Data::JsonURI& apConfigURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring WiFi as accesspoint..." << std::endl;
        try
        {
            json apConfigJson = apConfigURI.deserialize();
            const std::string& ssid = apConfigJson.at("ssid");
            const std::string& password = apConfigJson.at("password");

            WiFi.mode(WIFI_AP);
            WiFi.softAP(ssid.c_str(), password.c_str());
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure WiFi as accesspoint from \"" << apConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureWifi(const Data::JsonURI& wifiConfigURI)
    {
        if(!connectWifiStationary(wifiConfigURI / "/sta"_json_pointer))
            configureWifiAccesspoint(wifiConfigURI / "/ap"_json_pointer);
    }


    void addTrackerAPIHandlers(const Data::JsonURI& trackerConfigURI)
    {
        json jsonTrackingSpans = trackerConfigURI.deserialize();

        auto getFromJsonURI = [](const std::string& jsonURI, const json& data) {
            return Connectivity::RestAPI::JsonResponse(Data::JsonURI(jsonURI).deserialize());
        };

        using namespace std::placeholders;
        for(const auto& jsonTrackingSpan : jsonTrackingSpans)
        {
            Diagnostics::Logger[Level::Verbose] << jsonTrackingSpan.at("name") << std::endl;

            std::stringstream uri;
            uri << "/tracker/" << jsonTrackingSpan.at("name").get<std::string>() << std::endl;
            Diagnostics::Logger[Level::Verbose] << uri.str() << std::endl;
            api.handle(
                Connectivity::HTTP::Method::Get, 
                uri.str(), 
                std::bind(getFromJsonURI, "/lol", _1)
            );
        }
    }


    void addAPIHandlers()
    {
        Diagnostics::Logger[Level::Info] << "Adding API Handlers..." << std::endl;
        

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
        addTrackerAPIHandlers(Data::JsonURI(POWERMETER_TRACKER_CONFIG_URI));

        AsyncElegantOTA.begin(&server);
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