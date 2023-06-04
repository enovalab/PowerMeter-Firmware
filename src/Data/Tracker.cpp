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
    const JsonURI& lastSampleURI,
    const AverageAccumulator& accumulator
) : 
    m_title(title),
    m_duration_s(duration_s),
    m_sampleCount(sampleCount),
    m_clock(clock),
    m_dataURI(dataURI),
    m_lastSampleURI(lastSampleURI),
    m_accumulator(accumulator)
{}


void Tracker::track(float value)
{
    if(!isfinite(value))
        throw std::runtime_error("value must be finite number");
    
    m_accumulator.add(value);

    time_t lastSampleTimestamp;
    try
    {
        m_lastSampleURI.deserialize();
    }
    catch(...) 
    {
        Diagnostics::ExceptionTrace::clear();
        lastSampleTimestamp = m_clock.now();
        m_lastSampleURI.serialize(lastSampleTimestamp);
    }
    size_t timesElapsed = (m_clock.now() - lastSampleTimestamp) / (m_duration_s / m_sampleCount);
    
    for(size_t i = timesElapsed; i > 0; i--)
    {
        // Zeit in der das Gerät ausgeschaltet war mit 0 auffüllen
        float value = 0.0f;

        // Letzter (aktueller) Wert aus Akkumulator holen
        if(i == 1)
        {
            value = m_accumulator.getAverage();
            m_accumulator.reset();
            m_lastSampleURI.serialize(lastSampleTimestamp);
        }
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