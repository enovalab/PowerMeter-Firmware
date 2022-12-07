#include "Measuring/Tracker.h"
#include "Logging/Log.h"
#include <fstream>
#include <algorithm>

using namespace Measuring;


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
        time_t secondsPassed = now - config.m_lastSampleResource.deserialize();
        time_t secondsBetweenSamples = config.m_timeSpanSeconds / config.m_numSamplesPerSpan;
        if(secondsPassed > secondsBetweenSamples)
        {
            float newValue;
            if(i > 0)
            {
                newValue = averageJsonArray(m_configs[i - 1].m_targetResource);
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


void track(const System::JsonResource& targetResource, size_t maxArraySize, float newValue)
{
    try
    {
        json trackerArray = targetResource.deserialize();
        trackerArray.push_back(newValue);

        if(trackerArray.size() > maxArraySize)
            trackerArray.erase(trackerArray.begin());

        targetResource.serialize(trackerArray);
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


float averageJsonArray(const System::JsonResource& targetResource)
{
    try
    {
        json values = targetResource.deserialize();
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