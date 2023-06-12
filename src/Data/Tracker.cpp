#include "Data/Tracker.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"

#include "math.h"

using namespace Data;

Tracker::Tracker(
    const std::string& title,
    time_t duration_s,
    size_t sampleCount,
    const Time::Clock& clock,
    const JsonURI& dataURI,
    const JsonURI& lastInputURI,
    const JsonURI& lastSampleURI,
    const AverageAccumulator& accumulator
) : 
    m_title(title),
    m_duration_s(duration_s),
    m_sampleCount(sampleCount),
    m_clock(clock),
    m_dataURI(dataURI),
    m_lastInputURI(lastInputURI),
    m_lastSampleURI(lastSampleURI),
    m_accumulator(accumulator)
{}


void Tracker::track(float value)
{
    if(!isfinite(value))
        throw std::runtime_error("value must be finite number");

    time_t secondsSinceLastInput = m_clock.now() - getTimestamp(m_lastInputURI);

    if(secondsSinceLastInput <= 0)
        return;
    
    for(size_t i = 0; i < secondsSinceLastInput; i++)
    {
        m_lastInputURI.serialize(m_clock.now());
        m_accumulator.add(value);
    }

    
    uint32_t timesElapsed = (m_clock.now() - getTimestamp(m_lastSampleURI)) / (m_duration_s / m_sampleCount);

    if(timesElapsed > 0)
    {
        for(size_t i = 0; i < timesElapsed - 1; i++)
            updateData(0.0f);

        updateData(m_accumulator.getAverage());
        m_accumulator.reset();
    }
}


json Tracker::getData() const
{
    json data;
    data["title"] = m_title;
    data["sampleCount"] = m_sampleCount;
    data["duration_s"] = m_duration_s;
    try
    {
        data["data"] = m_dataURI.deserialize();
    }
    catch(...)
    {
        Diagnostics::ExceptionTrace::clear();
        data["data"] = json::array_t();
    }
    return data;
}


void Tracker::updateData(float value)
{
    json values;
    try
    {
        values = m_dataURI.deserialize();
    }                     
    catch(...)
    {
        Diagnostics::ExceptionTrace::clear();
    }
    values.push_back(value);

    if(values.size() > m_sampleCount)
        values.erase(values.begin());
    
    m_dataURI.serialize(values);
    m_lastSampleURI.serialize(m_clock.now());
}


time_t Tracker::getTimestamp(const JsonURI& timestampURI)
{
    try
    {
        return timestampURI.deserialize();
    }
    catch(...) 
    {
        Diagnostics::ExceptionTrace::clear();
        timestampURI.serialize(m_clock.now());
        return m_clock.now();
    }
}