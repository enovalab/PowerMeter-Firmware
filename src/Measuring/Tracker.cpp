#include "Measuring/Tracker.h"
#include "Logging/Log.h"
#include <fstream>
#include <algorithm>

using namespace Measuring;


TrackingSpan::TrackingSpan(
    const System::JsonResource& targetResource,
    const System::JsonResource& lastSampleResource, 
    const System::JsonResource& averageResource,
    time_t timeSpanSeconds, 
    size_t numSamplesPerSpan
) :
    m_targetResource(targetResource),
    m_lastSampleResource(lastSampleResource),
    m_averageResource(averageResource),
    m_timeSpanSeconds(timeSpanSeconds),
    m_numSamplesPerSpan(numSamplesPerSpan)
{}


void TrackingSpan::track(float newValue) const
{
    try
    {
        json trackerArray = m_targetResource.deserialize();
        trackerArray.push_back(newValue);

        if(trackerArray.size() > m_numSamplesPerSpan)
            trackerArray.erase(trackerArray.begin());

        m_targetResource.serialize(trackerArray);
    }
    catch(const std::exception& e)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
}


float TrackingSpan::average() const
{
    try
    {
        json values = m_averageResource.deserialize();
        float sum = 0;
        for(const json& value : values)
            sum += value;

        return sum / values.size();
    }
    catch(const std::exception& e)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
    return NAN;
}


time_t TrackingSpan::getTimeSpanSeconsds() const
{
    return m_timeSpanSeconds;
}


time_t TrackingSpan::getLastSampleTimestamp() const
{
    return m_lastSampleResource.deserialize();
}


size_t TrackingSpan::getNumSamplesPerSpan() const
{
    return m_numSamplesPerSpan;
}


Tracker::Tracker(const Time::IClock& clock, const std::vector<TrackingSpan>& configs) : 
    m_clock(clock),
    m_configs(configs)
{}


Tracker& Tracker::operator<<(float newValue)
{
    m_average << newValue;
    time_t now = m_clock.now();
    for(size_t i = 0; i < m_configs.size(); i++)
    {
        const TrackingSpan& config = m_configs[i];
        time_t secondsPassed = now - config.getLastSampleTimestamp();
        time_t secondsBetweenSamples = config.getTimeSpanSeconsds() / config.getNumSamplesPerSpan();
        if(secondsPassed > secondsBetweenSamples)
        {
            float newValue;
            if(i > 0)
            {
                newValue = config.average();
            }
            else
            {
                newValue = m_average;
                m_average.reset();
            }

            track(config.m_targetResource, config.m_numSamplesPerSpan, newValue);
            config.m_lastSampleResource.serialize(now);
        }
    }
    return *this;
}

