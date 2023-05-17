#ifndef DATA_TRACKER_H
#define DATA_TRACKER_H

#include "Time/Clock.h"
#include "Data/JsonURI.h"
#include "Data/AverageAccumulator.h"

namespace Data
{
    class Tracker
    {
    public:
        Tracker(
            time_t duration_s,
            size_t sampleCount,
            Time::Clock& clock,
            JsonURI dataURI,
            JsonURI lastSampleURI,
            AverageAccumulator accumulator
        );
        void track(float value);
        json getData() const;

    private:
        time_t getLastSampleTimestamp() const;
        void getLastSampleTimestamp(time_t);

        time_t m_duration_s;
        size_t m_sampleCount;
        Time::Clock& m_clock;
        JsonURI m_dataURI;
        JsonURI m_lastSampleURI;
        AverageAccumulator m_accumulator;
    };
}

#endif