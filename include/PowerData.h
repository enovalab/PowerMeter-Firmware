#ifndef PowerData_h
#define PowerData_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <array>
#include <initializer_list>
#include <LittleFS.h>

class PowerData
{
    public:
    PowerData(double value, uint32_t timestamp) : 
        value(value), 
        timestamp(timestamp)
    {}

    uint32_t timestamp;
    double value;
};

#endif