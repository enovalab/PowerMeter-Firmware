#if defined(ESP32) && !defined(PIO_UNIT_TESTING)

#include "PowerMeterAPI/PowerMeterAPI.h"
#include "Config/Config.h"
#include "Globals/Globals.h"
#include "Log/Log.h"
#include "ExceptionTrace/ExceptionTrace.h"
#include <LittleFS.h>


static bool WasBootSuccessful = true;


void setup()
{
    Serial.begin(115200);
    LittleFS.begin(true, "", 30);

    try
    {
        Config::configureLogger();
        Logger[Level::Info] << "Booting..." << std::endl;
        Config::configureWiFi();
        Globals::Server.start();
        Config::configureACPowerMeter();
        Config::configureRelay();
        Globals::RTC.begin();
        Config::configureTrackers();

        PowerMeterAPI::createSystemEndpoints();
        PowerMeterAPI::createLoggerEndpoints();
        PowerMeterAPI::createMeasuringEndpoints();
        PowerMeterAPI::createRelayEndpoints();
        PowerMeterAPI::createTrackerEndpoints();
        PowerMeterAPI::createWiFiEndpoints();
        Logger[Level::Info] << "Boot sequence finished. Running..." << std::endl;
    }
    catch(...)
    {
        Logger[Level::Error] 
            << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << ExceptionTrace::what() << std::endl;
    }
}

void loop()
{
    if(!WasBootSuccessful)
        return;
    try
    {
        ACPower power = Globals::PowerMeter.measure();

        for(auto& tracker : Globals::Trackers)
            tracker.second.track(power.getActivePower_W());

        delay(500);
    }
    catch(...)
    {
        Logger[Level::Error] 
            << SOURCE_LOCATION << "An Exception occurred, here is what happened:\n"
            << ExceptionTrace::what() << std::endl;
    }
}

#endif