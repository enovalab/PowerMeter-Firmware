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
            const std::string& title,
            time_t duration_s,
            size_t sampleCount,
            const Time::Clock& clock,
            const JsonURI& dataURI,
            const JsonURI& lastInputURI,
            const JsonURI& lastSampleURI,
            const AverageAccumulator& accumulator
        );
        void track(float value);
        json getData() const;

    private:
        void updateData(float value);
        time_t getTimestamp(const JsonURI& timestampURI);

        std::string m_title;
        time_t m_duration_s;
        size_t m_sampleCount;
        const Time::Clock& m_clock;
        JsonURI m_dataURI;
        JsonURI m_lastInputURI;
        JsonURI m_lastSampleURI;
        AverageAccumulator m_accumulator;
    };
}

#endif