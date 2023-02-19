#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"
#include "Data/JsonURI.h"

#include <Arduino.h>
#include <LittleFS.h>

void setup()
{
    Diagnostics::Logger[Level::Info] << ESP.getFreeHeap() << std::endl;
    try
    {
        Serial.begin(115200);
        LittleFS.begin(true);
        Data::JsonURI dataURI("/littlefs/PerformanceTest.json");
        uint32_t before = micros();

        json data = dataURI.deserialize();

        Diagnostics::Logger[Level::Info] << "took " << micros() - before << " µs" << std::endl;
        Diagnostics::Logger[Level::Info] << ESP.getFreeHeap() << std::endl;
    }
    catch(...)
    {
        Diagnostics::Logger[Level::Error] << Diagnostics::ExceptionTrace::what() << std::endl;
    }   
}

void loop()
{}