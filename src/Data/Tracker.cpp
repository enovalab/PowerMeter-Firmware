#include "Data/Tracker.h"
#include "Logging/Log.h"
#include "ErrorHandling/ExceptionTrace.h"

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
        json trackerArray;
        try
        {
            trackerArray = m_targetResource.deserialize();
        }
        catch(json::exception)
        {
            ErrorHandling::ExceptionTrace::clear();
        }
        catch(std::runtime_error)
        {
            ErrorHandling::ExceptionTrace::clear();
        }
        
        trackerArray.push_back(newValue);

        if(trackerArray.size() > m_numSamplesPerSpan)
            trackerArray.erase(trackerArray.begin());

        m_targetResource.serialize(trackerArray);
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to track \"" << newValue << "\"";
        ErrorHandling::ExceptionTrace::trace(errorMessage.str());
        throw;
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
    catch(json::exception)
    {
        ErrorHandling::ExceptionTrace::clear();
    }
    catch(std::runtime_error)
    {
        ErrorHandling::ExceptionTrace::clear();
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to average data at \"" << m_averageResource << "\""; 
        ErrorHandling::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
    return NAN;
}


time_t TrackingSpan::getTimeSpanSeconsds() const noexcept
{
    return m_timeSpanSeconds;
}


size_t TrackingSpan::getNumSamplesPerSpan() const noexcept
{
    return m_numSamplesPerSpan;
}


time_t TrackingSpan::getLastSampleTimestamp() const
{
    try
    {
        json lastSampleJson = m_lastSampleResource.deserialize();
        return lastSampleJson.get<time_t>();
    }
    catch(json::exception)
    {
        ErrorHandling::ExceptionTrace::clear();
    }
    catch(std::runtime_error)
    {
        ErrorHandling::ExceptionTrace::clear();
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to get timestamp from \"" << m_lastSampleResource << "\"";
        ErrorHandling::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
    return 0;
}


void TrackingSpan::setLastSampleTimestamp(time_t timestamp) const
{
    try
    {
        m_lastSampleResource.serialize(timestamp);
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to set timestamp to \"" << timestamp << "\"";
        ErrorHandling::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
    
}


Tracker::Tracker(const Time::Clock& clock, const std::vector<TrackingSpan>& trackingSpans) : 
    m_clock(clock),
    m_trackingSpans(trackingSpans)
{}


void Tracker::track(float newValue)
{
    try
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
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to track \"" << newValue << "\"";
        ErrorHandling::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}