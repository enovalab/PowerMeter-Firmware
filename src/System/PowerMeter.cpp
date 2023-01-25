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
    Measuring::ACPowerMeter powerMeter(0, 0);
    AsyncWebServer server(80);

    void configureLogger(const Data::JsonURI& configResource)
    {
        try
        {
            json configJson = configResource.deserialize();

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
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Diagnostics::Logger' from \"" << configResource << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureACPowerMeter(const Data::JsonURI& configResource)
    {
        try
        {
            json configJson = configResource.deserialize();
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
            errorMessage << SOURCE_LOCATION << "Failed to configure 'Measuring::PowerMeter' from \"" << configResource << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureRelay(const Data::JsonURI& configResource)
    {
        try
        {
            json configJson = configResource.deserialize();
            digitalWrite(configJson.at("pin"), configJson.at("state"));
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure Relay from \"" << configResource << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureWifiStationary(const Data::JsonURI& configResource)
    {
        try
        {
            json configJson = configResource.deserialize();

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
            
            WiFi.config(parseIP(staticIP), parseIP(gateway), parseIP(subnet), parseIP(primaryDNS), parseIP(secondaryDNS));
            WiFi.begin(ssid.c_str(), password.c_str());
            delay(500);
            if(WiFi.status() == WL_CONNECTED)
                Diagnostics::Logger[Level::Info] << "Sucessfully connected to " << ssid  << " IP: http://" << WiFi.localIP().toString().c_str() << std::endl;
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure WiFi as Stationary from \"" << configResource << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureWifiAccesspoint(const Data::JsonURI& configResource)
    {
        try
        {
            json configJson = configResource.deserialize();
            const std::string& ssid = configJson.at("ssid");
            const std::string& password = configJson.at("password");
            WiFi.softAP(ssid.c_str(), password.c_str());
        }
        catch(...)
        {
            std::stringstream errorMessage;
            errorMessage << SOURCE_LOCATION << "Failed to configure WiFi as Accesspoint from \"" << configResource << '"';
            Diagnostics::ExceptionTrace::trace(errorMessage.str());
            throw;
        }
    }


    void configureWifi(const Data::JsonURI& configResource)
    {
        WiFi.mode(WIFI_AP_STA);
        configureWifiAccesspoint(configResource / "/ap"_json_pointer);
        configureWifiStationary(configResource / "/sta"_json_pointer);
    }
}

void PowerMeter::init()
{
    try
    {
        Serial.begin(115200);
        LittleFS.begin(POWERMETER_FORMAT_FS_ON_FAIL);

        configureLogger(Data::JsonURI(POWERMETER_LOGGER_CONFIG_URI));
        configureWifi(Data::JsonURI(POWERMETER_WIFI_CONFIG_URI));
        configureACPowerMeter(Data::JsonURI(POWERMETER_ACPOWERMETER_CONFIG_URI));
        configureRelay(Data::JsonURI(POWERMETER_RELAY_CONFIG_URI));

        Connectivity::RestAPI api(&server, "/api");

        api.handle(Connectivity::HTTP::Method::Get, "/power", [](json){
            Measuring::ACPower power = powerMeter.measure();
            json data;
            data["voltage"] = power.getVoltageRms();
            data["current"] = power.getCurrentRms();
            data["active"] = power.getActivePower();
            data["apparent"] = power.getApparentPower();
            data["reactive"] = power.getReactivePower();
            data["powerFactor"] = power.getPowerFactor();
            return Connectivity::RestAPI::JsonResponse(data);
        });

        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last60min", [](json){
            json data = Data::JsonURI(POWERMETER_TRACKER_LAST60MIN_URI).deserialize();
            return Connectivity::RestAPI::JsonResponse(data);
        });

        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last24h", [](json){
            json data = Data::JsonURI(POWERMETER_TRACKER_LAST24H_URI).deserialize();
            return Connectivity::RestAPI::JsonResponse(data);
        });

        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last7d", [](json){
            json data = Data::JsonURI(POWERMETER_TRACKER_LAST7D_URI).deserialize();
            return Connectivity::RestAPI::JsonResponse(data);
        });

        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last30d", [](json){
            json data = Data::JsonURI(POWERMETER_TRACKER_LAST30D_URI).deserialize();
            return Connectivity::RestAPI::JsonResponse(data);
        });

        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last12m", [](json){
            json data = Data::JsonURI(POWERMETER_TRACKER_LAST12M_URI).deserialize();
            return Connectivity::RestAPI::JsonResponse(data);
        });

        api.handle(Connectivity::HTTP::Method::Get, "/tracker/last10a", [](json){
            json data = Data::JsonURI(POWERMETER_TRACKER_LAST10A_URI).deserialize();
            return Connectivity::RestAPI::JsonResponse(data);
        });

        server.serveStatic("/", LittleFS, "/app").setDefaultFile("index.html");

        server.begin();
    }
    catch(...)
    {
        Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << Diagnostics::ExceptionTrace::what() << std::endl;
    }
}


void PowerMeter::run()
{
    try
    {
        static Time::DS3231 clock;
        static Data::Tracker tracker(clock, {});

        tracker.track(powerMeter.measure().getActivePower());
    }
    catch(...)
    {
        Diagnostics::Logger[Level::Error] << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << Diagnostics::ExceptionTrace::what() << std::endl;
    }
}

#endif