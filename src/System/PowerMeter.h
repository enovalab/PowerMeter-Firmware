#ifndef SYSTEM_POWERMETER_H
#define SYSTEM_POWERMETER_H

#include "Logging/Log.h"
#include "Hardware/PowerMeter.h"
#include "Measuring/Tracker.h"
#include "Time/DS3231.h"
#include "System/JsonResource.h"

namespace System
{
    class PowerMeter
    {
    public:
        PowerMeter(std::string configPath);
        void run();

    private:
        Hardware::PowerMeter m_powerMeter;
        Time::DS3231 m_clock;
        Measuring::Tracker m_tracker;
    };

    Logging::Log configureLogger(const JsonResource& configResource);

}

#endif