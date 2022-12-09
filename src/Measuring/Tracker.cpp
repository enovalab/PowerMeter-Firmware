#include "Measuring/Tracker.h"
#include "Logging/Log.h"
#include <fstream>
#include <algorithm>

using namespace Measuring;


TrackingSpan::TrackingSpan(
    const System::JsonResource& targetResource,
    const System::JsonResource& lastSampleResource,
    time_t timeSpanSeconds, 
    size_t numSamplesPerSpan,
    const System::JsonResource& averageResource
) :
    m_targetResource(targetResource),
    m_timeSpanSeconds(timeSpanSeconds),
    m_numSamplesPerSpan(numSamplesPerSpan),
    m_lastSampleResource(lastSampleResource),
    m_averageResource(averageResource)
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
            sum += value.get<float>();

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


size_t TrackingSpan::getNumSamplesPerSpan() const
{
    return m_numSamplesPerSpan;
}


time_t TrackingSpan::getLastSampleTimestamp() const
{
    json lastSampleJson = m_lastSampleResource.deserialize();
    try
    {
        if(!lastSampleJson.is_null())
            return lastSampleJson.get<time_t>();
    }
    catch(const std::exception& e)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
    return 0;
}


void TrackingSpan::setLastSampleTimestamp(time_t timestamp) const
{
    m_lastSampleResource.serialize(timestamp);
}


Tracker::Tracker(const Time::IClock& clock, const std::vector<TrackingSpan>& configs) : 
    m_clock(clock),
    m_configs(configs)
{}


void Tracker::track(float newValue)
{
    m_average << newValue;
    time_t now = m_clock.now();
    for(size_t i = 0; i < m_configs.size(); i++)
    {
        const TrackingSpan& config = m_configs[i];
        time_t secondsPassed = now - config.getLastSampleTimestamp();
        time_t secondsBetweenSamples = config.getTimeSpanSeconsds() / config.getNumSamplesPerSpan();
        size_t timesElapsed = secondsPassed / secondsBetweenSamples;

        Logging::Logger[Logging::Level::Debug] << secondsPassed << " s passed since last sample" << std::endl;
        Logging::Logger[Logging::Level::Debug] << timesElapsed << " times elapsed" << std::endl;

        for(size_t j = timesElapsed; j > 0; j--)
        {
            float newValue;
            if(j == 1)
            {
                if(i > 0)
                {
                    newValue = config.average();
                    Logging::Logger[Logging::Level::Debug] << "Got average from file" << std::endl;
                }
                else
                {
                    newValue = m_average;
                    m_average.reset();
                    Logging::Logger[Logging::Level::Debug] << "Got average from StreamAverage" << std::endl;
                }
                config.setLastSampleTimestamp(now);
                Logging::Logger[Logging::Level::Debug] << "Last sample Timestamp set to " << now << std::endl;
            }
            else
            {
                newValue = 0.0f;
                Logging::Logger[Logging::Level::Debug] << "New value set to 0" << std::endl;
            }

            config.track(newValue);
            Logging::Logger[Logging::Level::Debug] << "New value" << newValue << " has been tracked" << std::endl;
        }
    }
}

