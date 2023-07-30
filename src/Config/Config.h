#ifndef CONFIG_H
#define CONFIG_H

#include "JsonURI/JsonURI.h"
#include "ACPowerMeter/ACPowerMeter.h"

namespace Config
{
    void configureLogger();
    void configureACPowerMeter();
    void configureRelay();
    void configureTrackers();
    bool configureWiFiStationary();
    bool configureWifiAccesspoint();
    void configureWiFi();
}

#endif

