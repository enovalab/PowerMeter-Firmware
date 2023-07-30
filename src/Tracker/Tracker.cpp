#include "Tracker.h"
#include "Log/Log.h"
#include "ExceptionTrace/ExceptionTrace.h"

#include <math.h>
#include <sstream>


Tracker::Tracker(
    const std::string& title,
    time_t duration_s,
    size_t sampleCount,
    const Clock& clock,
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
        if(!isfinite(value))
            value = 0.0f;

        time_t lastInputTimestamp = getTimestamp(m_lastInputURI);
        time_t secondsSinceLastInput = m_clock.now() - lastInputTimestamp;

        if(secondsSinceLastInput <= 0)
            return;
        
        for(size_t i = 0; i < secondsSinceLastInput; i++)
        {
            m_lastInputURI.serialize(m_clock.now());
            m_accumulator.add(value);
        }
        
        time_t lastSampleTimestamp = getTimestamp(m_lastSampleURI);
        uint32_t timesElapsed = (m_clock.now() - lastSampleTimestamp) / (m_duration_s / m_sampleCount);

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
        ExceptionTrace::trace(errorMessage.str());
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
            ExceptionTrace::clear();
            data["data"] = json::array_t();
        }
        return data;
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to get Data";
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void Tracker::setData(const json& data)
{
    try
    {
        m_dataURI.serialize(data.at("data"));
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to set Data";
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void Tracker::erase()
{
    m_dataURI.erase();
    m_lastInputURI.erase();
    m_lastSampleURI.erase();
    m_accumulator.erase();
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
            ExceptionTrace::clear();
        }
        values.push_back(value);

        if(values.size() > m_sampleCount)
            values.erase(values.begin());
        
        m_dataURI.serialize(values);
        m_lastSampleURI.serialize(m_clock.now());
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to update Data " << value;
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


time_t Tracker::getTimestamp(const JsonURI& timestampURI) const
{
    try
    {
        return timestampURI.deserialize();
    }
    catch(...) 
    {
        ExceptionTrace::clear();
        timestampURI.serialize(m_clock.now());
        return m_clock.now();
    }
}