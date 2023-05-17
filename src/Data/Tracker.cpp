#include "Data/Tracker.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"

#include "math.h"

using namespace Data;

Tracker::Tracker(
    time_t duration_s,
    size_t sampleCount,
    Time::Clock& clock,
    JsonURI dataURI,
    JsonURI lastSampleURI,
    AverageAccumulator accumulator
) : 
    m_duration_s(duration_s),
    m_sampleCount(sampleCount),
    m_clock(clock),
    m_dataURI(dataURI),
    m_lastSampleURI(lastSampleURI),
    m_accumulator(accumulator)
{
    if(m_lastSampleURI.deserialize().is_null())
        m_lastSampleURI.serialize(m_clock.now());
}


void Tracker::track(float value)
{
    try
    {
        if(!isfinite(value))
            throw std::runtime_error("value must be finite number");
        
        m_accumulator.add(value);

        time_t lastSampleTimestamp = m_lastSampleURI.deserialize();
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
            }                        
        }
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to track \"" << value << "\"";
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}