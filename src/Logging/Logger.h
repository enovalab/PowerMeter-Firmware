#ifndef LOGGER_H
#define LOGGER_H

#include <ArduinoLog.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

namespace Logger
{
    Logging& Log = ::Log;
    
    void begin(uint32_t baudRate = 115200, const char* logFilePath = "log/log.log")
    {
        Serial.begin(baudRate);
        while(!SPIFFS.begin());
        File configFile = SPIFFS.open("/data/config.json");
        StaticJsonDocument<100> configJson;
        StaticJsonDocument<50> filterJson;
        filterJson["logging"] = true;
        deserializeJson(configJson, configFile, DeserializationOption::Filter(filterJson));

        uint8_t logLevel = configJson["logging"]["level"];
        bool logToFile = configJson["logging"]["file"];
        Print* logStream;
        if(logToFile)
        {
            if(SPIFFS.exists(logFilePath)) 
                SPIFFS.remove(logFilePath);
            File logFile = SPIFFS.open(logFilePath, FILE_APPEND, true);
            logStream = &logFile;
        }
        else
        {
            logStream = &Serial;
        }
        Log.begin(logLevel, logStream);
    }
}

#endif