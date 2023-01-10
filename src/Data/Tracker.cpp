#include "Data/Tracker.h"
#include "Logging/Log.h"

#include <fstream>
#include <exception>
#include <algorithm>

using namespace Data;


TrackingSpan::TrackingSpan(
    const Data::JsonURI& targetResource,
    const Data::JsonURI& lastSampleResource,
    time_t timeSpanSeconds, 
    size_t numSamplesPerSpan,
    const Data::JsonURI& averageResource
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
    catch(const json::out_of_range& e)
    {
        json trackerArray;
        trackerArray.push_back(newValue);
        m_targetResource.serialize(trackerArray);
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to track \"" << newValue << "\"";
        std::throw_with_nested(std::runtime_error(errorMessage.str()));
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
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to average data at \"" << m_averageResource << "\""; 
        std::throw_with_nested(std::runtime_error(errorMessage.str()));
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
    try
    {
        json lastSampleJson = m_lastSampleResource.deserialize();

        if(!lastSampleJson.is_null())
            return lastSampleJson.get<time_t>();
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to get timestamp from \"" << m_lastSampleResource << "\"";
        std::throw_with_nested(std::runtime_error(errorMessage.str()));
    }
    return 0;
}


void TrackingSpan::setLastSampleTimestamp(time_t timestamp) const
{
    m_lastSampleResource.serialize(timestamp);
}


Tracker::Tracker(const Time::Clock& clock, const std::vector<TrackingSpan>& trackingSpans) : 
    m_clock(clock),
    m_trackingSpans(trackingSpans)
{}


void Tracker::track(float newValue)
{
    m_average << newValue;
    time_t now = m_clock.now();

    for(size_t i = 0; i < m_trackingSpans.size(); i++)
    {
        const TrackingSpan& trackingSpan = m_trackingSpans[i];
        time_t secondsPassed = now - trackingSpan.getLastSampleTimestamp();
        time_t secondsBetweenSamples = trackingSpan.getTimeSpanSeconsds() / trackingSpan.getNumSamplesPerSpan();
        size_t timesElapsed = secondsPassed / secondsBetweenSamples;

        for(size_t j = timesElapsed; j > 0; j--)
        {
            float newValue;
            if(j == 1)
            {
                if(i > 0)
                {
                    newValue = trackingSpan.average();
                }
                else
                {
                    newValue = m_average;
                    m_average.reset();
                }
                trackingSpan.setLastSampleTimestamp(now);
            }
            else
            {
                newValue = 0.0f;
            }

            trackingSpan.track(newValue);
        }
    }
}