#ifndef TRACKER_H
#define TRACKER_H

#include "Clock/Clock.h"
#include "JsonURI/JsonURI.h"
#include "AverageAccumulator/AverageAccumulator.h"


class Tracker
{
public:
    Tracker(
        const std::string& title,
        time_t duration_s,
        size_t sampleCount,
        const Clock& clock,
        const JsonURI& dataURI,
        const JsonURI& lastInputURI,
        const JsonURI& lastSampleURI,
        const AverageAccumulator& accumulator
    ) noexcept;
    void track(float value);
    json getData() const;
    void setData(const json& data);
    void erase();

private:
    void updateData(float value);
    time_t getTimestamp(const JsonURI& timestampURI) const;

    std::string m_title;
    time_t m_duration_s;
    size_t m_sampleCount;
    const Clock& m_clock;
    JsonURI m_dataURI;
    JsonURI m_lastInputURI;
    JsonURI m_lastSampleURI;
    AverageAccumulator m_accumulator;
};

#endif