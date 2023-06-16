#include "Data/Tracker.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"

#include <math.h>
#include <sstream>

using namespace Data;

Tracker::Tracker(
    const std::string& title,
    time_t duration_s,
    size_t sampleCount,
    const Time::Clock* clock,
    const JsonURI& dataURI,
    const JsonURI& lastInputURI,
    const JsonURI& lastSampleURI,
    const AverageAccumulator& accumulator
) noexcept : 
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
    try
    {
        Diagnostics::Logger[Level::Debug] << "Tracking..." << std::endl;
        if(!isfinite(value))
            value = 0.0f;

        time_t lastInputTimestamp = getTimestamp(m_lastInputURI);
        time_t secondsSinceLastInput = m_clock->now() - lastInputTimestamp;
        Diagnostics::Logger[Level::Debug] << "now: " << m_clock->now() << std::endl;
        Diagnostics::Logger[Level::Debug] << "lastInputTimestamp: " << lastInputTimestamp << std::endl;

        if(secondsSinceLastInput <= 0)
            return;
        
        for(size_t i = 0; i < secondsSinceLastInput; i++)
        {
            m_lastInputURI.serialize(m_clock->now());
            m_accumulator.add(value);
        }
        
        time_t lastSampleTimestamp = getTimestamp(m_lastSampleURI);
        uint32_t timesElapsed = (m_clock->now() - lastSampleTimestamp) / (m_duration_s / m_sampleCount);

        Diagnostics::Logger[Level::Debug] << "secondsSinceLastInput: " << secondsSinceLastInput << std::endl;
        Diagnostics::Logger[Level::Debug] << "timesElapsed: " << timesElapsed << std::endl;
        Diagnostics::Logger[Level::Debug] << "lastSampleTimestamp: " << lastSampleTimestamp << std::endl;

        if(timesElapsed > 0)
        {
            for(size_t i = 0; i < timesElapsed - 1; i++)
                updateData(0.0f);

            updateData(m_accumulator.getAverage());
            m_accumulator.reset();
        }
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to track " << value;
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


json Tracker::getData() const
{
    try
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
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to get Data";
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void Tracker::updateData(float value)
{
    try
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
        Diagnostics::Logger[Level::Debug] << "updated Tracker Data: " << values;
        m_lastSampleURI.serialize(m_clock->now());
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to update Data " << value;
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
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
        timestampURI.serialize(m_clock->now());
        return m_clock->now();
    }
}