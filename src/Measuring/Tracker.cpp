#include "Measuring/Tracker.h"
#include "Logging/Log.h"
#include <fstream>
#include <algorithm>

using namespace Measuring;


TrackingSpan::TrackingSpan(
    const System::JsonResource& jsonResource, 
    time_t timeSpanSeconds, 
    size_t numSamplesPerSpan, 
    time_t lastSampleTimestamp
) : 
    m_targetResource(jsonResource),
    m_timeSpanSeconds(timeSpanSeconds),
    m_numSamplesPerSpan(numSamplesPerSpan),
    m_lastSampleTimestamp(lastSampleTimestamp)
{}


System::JsonResource TrackingSpan::getTargetResource() const
{
    return m_targetResource;
}


time_t TrackingSpan::getTimeSpanSeconds() const
{
    return m_timeSpanSeconds;
}


size_t TrackingSpan::getNumSamplesPerSpan() const
{
    return m_numSamplesPerSpan;
}


time_t TrackingSpan::getLastSampleTimestamp() const
{
    return m_lastSampleTimestamp;
}


void TrackingSpan::pushBackPopFront(float newValue) const
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


Tracker::Tracker(const Time::IClock& clock, const std::vector<TrackingSpan>& configs) : 
    m_clock(clock),
    m_configs(configs)
{}

Tracker& Tracker::operator<<(float newValue)
{
    m_average << newValue;
    for(size_t i = 0; i < m_configs.size(); i++)
    {
        time_t timePassed = m_clock.now() - m_configs[i].getLastSampleTimestamp();
        time_t timeBetweenSamples = m_configs[i].getTimeSpanSeconds() / m_configs[i].getNumSamplesPerSpan();
        if(timePassed > timeBetweenSamples)
        {
            if(i == 0)
            {
                m_configs[i].pushBackPopFront(m_average);
                m_average.reset();
            }
            else
            {
                m_configs[i].pushBackPopFront(getJsonArrayAverage(m_configs[i - 1].getTargetResource()));
            }
        }
    }
    return *this;
}


float Tracker::getJsonArrayAverage(const System::JsonResource& jsonResource)
{
    json items = jsonResource.deserialize();
    float sum = 0;
    for(const json& item : items)
        sum += item;

    return sum / items.size();
}