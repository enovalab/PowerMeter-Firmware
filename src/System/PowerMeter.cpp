#ifdef ESP32

#include "System/PowerMeter.h"
#include <SPIFFS.h>
#include <fstream>

using namespace System;

void PowerMeter::init()
{
    Serial.begin(115200);
    if(SPIFFS.begin(true))
        Logging::Logger[Level::Warning] << SOURCE_LOCATION << "Formatted SPIFFS, after failed to mount" << std::endl;

    configureLogger(Data::JsonURI(""));
    configureWifi(Data::JsonURI(""));
    configurePowerMeter(Data::JsonURI(""));
}


void PowerMeter::run()
{
    m_tracker.track(m_powerMeter.measure().getActivePower());
}


void PowerMeter::configureLogger(const Data::JsonURI& configResource)
{
    try
    {
        json configJson = configResource.deserialize();

        Serial.begin(configJson.at("baud"));
        Logging::Logger.setLevel(Logging::Log::getLevelByName(configJson.at("level")));
        Logging::Logger.setShowLevel(configJson.at("showLevel"));

        if(configJson.at("fileMode"))
        {
            static std::ofstream logFile(configJson.at("logFile"));
            Logging::Logger.setOutputStream(&logFile);
        }
    }
    catch(const std::exception& e)
    {
        Logging::Logger[Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
}


void PowerMeter::configurePowerMeter(const Data::JsonURI& configResource)
{
    try
    {
        json configJson = configResource.deserialize();
        m_powerMeter = Measuring::PowerMeter(
            configJson.at("/pins/voltage"_json_pointer), 
            configJson.at("/pins/current"_json_pointer)
        );

        m_powerMeter.calibrate(
            configJson.at("/calibration/voltage"_json_pointer),
            configJson.at("/calibration/current"_json_pointer),
            configJson.at("/calibration/phase"_json_pointer)
        );
    }
    catch(const std::exception& e)
    {
        Logging::Logger[Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
}


void PowerMeter::configureRelay(const Data::JsonURI& configResource)
{
    try
    {
        json configJson = configResource.deserialize();
        digitalWrite(configJson.at("pin"), configJson.at("state"));
    }
    catch(const std::exception& e)
    {
        Logging::Logger[Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
}


void PowerMeter::configureWifi(const Data::JsonURI& configResource)
{
    WiFi.mode(WIFI_AP_STA);
    configureWifiAccesspoint(configResource / "/ap"_json_pointer);
    configureWifiStationary(configResource / "/sta"_json_pointer);
}


void PowerMeter::configureWifiAccesspoint(const Data::JsonURI& configResource)
{
    try
    {
        json configJson = configResource.deserialize();
        const std::string& ssid = configJson.at("ssid");
        const std::string& password = configJson.at("password");
        WiFi.softAP(ssid.c_str(), password.c_str());
    }
    catch(const std::exception& e)
    {
        Logging::Logger[Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
}


void PowerMeter::configureWifiStationary(const Data::JsonURI& configResource)
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
                Logging::Logger[Level::Warning] << SOURCE_LOCATION << "Could not convert \"" << address << "\" to 'IPAdress'" << std::endl;
            return ip;
        };
        
        WiFi.config(parseIP(staticIP), parseIP(gateway), parseIP(subnet), parseIP(primaryDNS), parseIP(secondaryDNS));
        WiFi.begin(ssid.c_str(), password.c_str());
    }
    catch(const std::exception& e)
    {
        Logging::Logger[Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
}

#endif