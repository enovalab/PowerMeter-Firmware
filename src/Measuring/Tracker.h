#ifndef TRACKER_H
#define TRACKER_H

#include "Time/Clock.h"
#include "Logging/Logger.h"

#include <StreamAverage.h>
#include <string>
#include <chrono>

namespace Measuring
{
    class Tracker
    {
    public:
        Tracker(const Time::Clock& clock, std::string filePath, time_t interval, size_t samplesPerInterval);
        Tracker& operator<<(float newValue);
    
    private:
        void pushBack(float newValue);

        const Time::Clock& m_clock;
        std::string m_filePath;
        time_t m_interval;
        size_t m_samplesPerInterval;
        time_t m_lastSample = 0;
        StreamAverage<float> average;
    };
}

#endif