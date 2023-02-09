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
using namespace std::placeholders;

namespace
{
    Measuring::ACPowerMeter powerMeter(33, 32);
    Measuring::ACPower power(0, 0, 0);
    AsyncWebServer server(80);
    Connectivity::RestAPI api(&server, "/api");
    Time::DS3231 rtc;
    Data::Tracker tracker(rtc);


    Connectivity::RestAPI::JsonResponse handleGetJsonURI(const Data::JsonURI& jsonURI, json)
    {
        return Connectivity::RestAPI::JsonResponse(jsonURI.deserialize());
    }


    Connectivity::RestAPI::JsonResponse handlePutJsonURI(const Data::JsonURI& jsonURI, const json& data)
    {
        jsonURI.serialize(data);
        return Connectivity::RestAPI::JsonResponse(jsonURI.deserialize());
    }


    Connectivity::RestAPI::JsonResponse handlePatchJsonURI(const Data::JsonURI& jsonURI, const json& data)
    {
        json storedData = jsonURI.deserialize();
        storedData.update(data, true);
        jsonURI.serialize(storedData);
        return Connectivity::RestAPI::JsonResponse(jsonURI.deserialize());
    }


    void configureLogger(const Data::JsonURI& loggerConfigURI)
    {
        try
        {
            Diagnostics::Logger[Level::Info] << "Configuring Logger..." << std::endl;
            json loggerConfigJson = loggerConfigURI.deserialize();

            Serial.begin(loggerConfigJson.at("baudRate"));
            Diagnostics::Logger.setLevel(Diagnostics::Log::getLevelByName(loggerConfigJson.at("level")));
            Diagnostics::Logger.setShowLevel(loggerConfigJson.at("showLevel"));
            const std::string& logFilePath = loggerConfigJson.at("logFile");

            static std::ofstream logFile;
            if(loggerConfigJson.at("fileMode"))
            {
                logFile.open(logFilePath);
                Diagnostics::Logger.setOutputStream(&logFile);
            }
            
            server.on("/log", HTTP_GET, [logFilePath](AsyncWebServerRequest* request){
                logFile.close();
                request->send(LittleFS, logFilePath.substr(9).c_str(), "text/plain");
                logFile.open(logFilePath, std::ios::app);
            });

            api.handle(Connectivity::HTTP::Method::Get, "/config/logger", std::bind(handleGetJsonURI, loggerConfigURI, _1));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/logger", [loggerConfigURI](const json& data){
                const Connectivity::RestAPI::JsonResponse& jsonResponse = handlePatchJsonURI(loggerConfigURI, data);
                configureLogger(loggerConfigURI);
                return jsonResponse;
            });

            Diagnostics::Logger[Level::Info] << "Logger configured sucessfully." << std::endl;
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Diagnostics::Logger' from \"" << loggerConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureMeasuring(const Data::JsonURI& measuringConfigURI)
    {
        Diagnostics::Logger[Level::Info] << "Configuring measuring unit..." << std::endl;
        try
        {
            json powerMeterConfigJson = measuringConfigURI.deserialize();

            uint16_t pinU = powerMeterConfigJson.at("/pins/voltage"_json_pointer); 
            uint16_t pinI = powerMeterConfigJson.at("/pins/current"_json_pointer);

            powerMeter = Measuring::ACPowerMeter(pinU, pinI);

            float calU = powerMeterConfigJson.at("/calibration/voltage"_json_pointer);
            float calI = powerMeterConfigJson.at("/calibration/current"_json_pointer);
            int16_t calPhase = powerMeterConfigJson.at("/calibration/phase"_json_pointer);

            powerMeter.calibrate(calU, calI, calPhase);

            api.handle(Connectivity::HTTP::Method::Get, "/power", [](json){
                json data;
                Measuring::ACPower power(0, 0, 0);
                power = powerMeter.measure();
                data["voltage"] = power.getVoltageRms();
                data["current"] = power.getCurrentRms();
                data["active"] = power.getActivePower();
                data["apparent"] = power.getApparentPower();
                data["reactive"] = power.getReactivePower();
                data["powerFactor"] = power.getPowerFactor();
                return Connectivity::RestAPI::JsonResponse(data);
            });
            api.handle(Connectivity::HTTP::Method::Get, "/config/measuring", std::bind(handleGetJsonURI, measuringConfigURI, _1));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/measuring", [measuringConfigURI](const json& data){
                const Connectivity::RestAPI::JsonResponse& jsonResponse = handlePatchJsonURI(measuringConfigURI, data);
                configureMeasuring(measuringConfigURI);
                return jsonResponse;
            });

            Diagnostics::Logger[Level::Info] << "Measuring unit configured sucessfully." << std::endl;
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Measuring::PowerMeter' from \"" << measuringConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureRelay(const Data::JsonURI& relayConfigURI)
    {
        try
        {
            Diagnostics::Logger[Level::Info] << "Configuring relay..." << std::endl;

            json relayConfigJson = relayConfigURI.deserialize();
            const uint16_t& pin = relayConfigJson.at("pin");;
            bool state = relayConfigJson.at("state");
            pinMode(pin, OUTPUT);
            digitalWrite(pin, state);

            api.handle(Connectivity::HTTP::Method::Get, "/config/relay", std::bind(handleGetJsonURI, relayConfigURI, _1));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/relay", [relayConfigURI](const json& data){
                const Connectivity::RestAPI::JsonResponse& jsonResponse = handlePatchJsonURI(relayConfigURI, data);
                json relayConfigJson = relayConfigURI.deserialize();
                const uint16_t& pin = relayConfigJson.at("pin");;
                bool state = relayConfigJson.at("state");
                pinMode(pin, OUTPUT);
                digitalWrite(pin, state);
                return jsonResponse;
            });

            Diagnostics::Logger[Level::Info] << "Relay configured sucessfully." << std::endl;
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
            Diagnostics::Logger[Level::Info] << "Configuring tracker..." << std::endl;
            json jsonTrackingSpans = trackerConfigURI.deserialize();
            std::vector<Data::TrackingSpan> trackingSpans;
            for(const auto& jsonTrackingSpan : jsonTrackingSpans.items())
            {
                const Data::JsonURI& targetURI = Data::JsonURI(jsonTrackingSpan.value().at("targetURI"));
                const Data::JsonURI& lastSampleURI = Data::JsonURI(jsonTrackingSpan.value().at("lastSampleURI"));
                uint32_t timeSpanSeconds = jsonTrackingSpan.value().at("timeSpanSeconds");
                uint32_t numSamplesPerSpan = jsonTrackingSpan.value().at("numSamplesPerSpan");
                const Data::JsonURI& averageURI = Data::JsonURI(jsonTrackingSpan.value().value("averageURI", ""));

                trackingSpans.push_back(Data::TrackingSpan(targetURI, lastSampleURI, timeSpanSeconds, numSamplesPerSpan, averageURI));

                api.handle(
                    Connectivity::HTTP::Method::Get, 
                    std::string("/tracker/") + jsonTrackingSpan.key(), 
                    std::bind(handleGetJsonURI, Data::JsonURI(jsonTrackingSpan.value().at("targetURI")), _1)
                );
            }
            tracker.setTrackingSpans(trackingSpans);
            if(tracker.init())
                Diagnostics::Logger[Level::Info] << "Initialized last sample timestamps" << std::endl;

            api.handle(Connectivity::HTTP::Method::Get, "/config/tracker", std::bind(handleGetJsonURI, trackerConfigURI, _1));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/tracker", [trackerConfigURI](const json& data){
                const Connectivity::RestAPI::JsonResponse& jsonResponse = handlePatchJsonURI(trackerConfigURI, data);
                configureTracker(trackerConfigURI);
                return jsonResponse;
            });

            Diagnostics::Logger[Level::Info] << "Tracker configured sucessfully." << std::endl;
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Data::Tracker' from \"" << trackerConfigURI << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    bool configureWifiStationary(const Data::JsonURI& staConfigURI)
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

            api.handle(Connectivity::HTTP::Method::Get, "/config/wifi/sta", std::bind(handleGetJsonURI, staConfigURI, _1));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/wifi/sta", [staConfigURI](const json& data){
                const Connectivity::RestAPI::JsonResponse& jsonResponse = handlePatchJsonURI(staConfigURI, data);
                configureWifiStationary(staConfigURI);
                return jsonResponse;
            });

            if(WiFi.waitForConnectResult() == WL_CONNECTED)
            {
                Diagnostics::Logger[Level::Info] 
                    << "Sucessfully connected to " 
                    << ssid  
                    << " IP: http://" 
                    << WiFi.localIP().toString().c_str() 
                    << std::endl;

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

            api.handle(Connectivity::HTTP::Method::Get, "/config/wifi/ap", std::bind(handleGetJsonURI, apConfigURI, _1));
            api.handle(Connectivity::HTTP::Method::Patch, "/config/wifi/ap", [apConfigURI](const json& data){
                const Connectivity::RestAPI::JsonResponse& jsonResponse = handlePatchJsonURI(apConfigURI, data);
                configureWifiAccesspoint(apConfigURI);
                return jsonResponse;
            });
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
        Diagnostics::Logger[Level::Info] << "Configuringn WiFi..." << std::endl;
        if(!configureWifiStationary(wifiConfigURI / "/sta"_json_pointer))
        {
            configureWifiAccesspoint(wifiConfigURI / "/ap"_json_pointer);
            Diagnostics::Logger[Level::Info] << "Couldn't connect to stationary network. Setting up acesspoint..." << std::endl;
        }
        Diagnostics::Logger[Level::Info] << "WiFi configured sucessfully." << std::endl;
    }

}

void PowerMeter::init() noexcept
{
    Serial.begin(115200);
    LittleFS.begin(POWERMETER_FORMAT_FS_ON_FAIL, "/littlefs", 30);
    AsyncElegantOTA.begin(&server);
    rtc.begin();
    server.serveStatic("/", LittleFS, "/app").setDefaultFile("index.html");

    api.handle(Connectivity::HTTP::Method::Get, "/reboot", [](json){
        Diagnostics::Logger[Level::Info] << "Rebooting Power Meter..." << std::endl;
        ESP.restart();
        return Connectivity::RestAPI::JsonResponse();
    });

    api.handle(Connectivity::HTTP::Method::Get, "/info", [](json){
        
        return Connectivity::RestAPI::JsonResponse();
    });

    
    try
    {
        configureLogger(Data::JsonURI(POWERMETER_LOGGER_CONFIG_URI));
        Diagnostics::Logger[Level::Info] << "Booting..." << std::endl;
        configureWifi(Data::JsonURI(POWERMETER_WIFI_CONFIG_URI));
        configureMeasuring(Data::JsonURI(POWERMETER_MEASURING_CONFIG_URI));
        configureRelay(Data::JsonURI(POWERMETER_RELAY_CONFIG_URI));
        configureTracker(Data::JsonURI(POWERMETER_TRACKER_CONFIG_URI));
    }
    catch(...)
    {
        Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << Diagnostics::ExceptionTrace::what() << std::endl;
    }
    Diagnostics::Logger[Level::Info] << "Boot sequence finished. Running..." << std::endl;
    server.begin();
}


void PowerMeter::run() noexcept
{
    try
    {
        
    }
    catch(...)
    {
        Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << Diagnostics::ExceptionTrace::what() << std::endl;
    }
}

#endif