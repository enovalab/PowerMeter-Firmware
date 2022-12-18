#ifndef SYSTEM_POWERMETER_H
#define SYSTEM_POWERMETER_H

#include "Logging/Log.h"
#include "Measuring/PowerMeter.h"
#include "Data/Tracker.h"
#include "Time/DS3231.h"
#include "Data/JsonURI.h"
#include <ESPAsyncWebServer.h>

namespace System
{
    class PowerMeter
    {
    public:
        void init();
        void run();

    private:
        void configureLogger(const Data::JsonURI& configResource);
        void configurePowerMeter(const Data::JsonURI& configResource);
        void configureRelay(const Data::JsonURI& configResource);
        void configureWifi(const Data::JsonURI& configResource);
        void configureWifiAccesspoint(const Data::JsonURI& configResource);
        void configureWifiStationary(const Data::JsonURI& configResource);

        Measuring::PowerMeter m_powerMeter;
        Time::DS3231 m_clock;
        Data::Tracker m_tracker;
        AsyncWebServer m_server;
    };
}

#endif