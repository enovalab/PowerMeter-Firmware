#ifndef GLOBALS_H
#define GLOBALS_H

#include "ACPowerMeter/ACPowerMeter.h"
#include "DS3231/DS3231.h"
#include "Tracker/Tracker.h"
#include "HTTPSServer/HTTPSServer.h"
#include "RestAPI/RestAPI.h"
#include <unordered_map>


namespace Globals
{
    extern const std::string FirmwareVersion;
    extern const JsonURI LoggerConfigURI;
    extern const JsonURI WiFiConfigURI;
    extern const JsonURI MeasuringConfigURI;
    extern const JsonURI TrackersConfigURI;
    extern const JsonURI RelayConfigURI;
    extern const JsonURI RelayStateURI;

    extern ACPowerMeter PowerMeter;
    extern DS3231 RTC;
    extern std::unordered_map<std::string, Tracker> Trackers;
    extern HTTPSServer Server;
    extern RestAPI API;
}

#endif