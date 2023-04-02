#include "Data/Tracker.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"

#include <fstream>
#include <exception>
#include <algorithm>
#include <math.h>

using namespace Data;


TrackingSpan::TrackingSpan(
    const Data::JsonURI& targetResource,
    const Data::JsonURI& lastSampleResource,
    time_t timeSpanSeconds, 
    size_t numSamplesPerSpan,
    const Data::JsonURI& averageResource
) :
    m_targetURI(targetResource),
    m_timeSpanSeconds(timeSpanSeconds),
    m_numSamplesPerSpan(numSamplesPerSpan),
    m_lastSampleURI(lastSampleResource),
    m_averageURI(averageResource)
{}


void TrackingSpan::track(float newValue) const
{
    try
    {
        json trackerArray;
        try
        {
            trackerArray = m_targetURI.deserialize();
        }
        catch(json::exception)
        {
            Diagnostics::ExceptionTrace::clear();
        }
        catch(std::runtime_error)
        {
            Diagnostics::ExceptionTrace::clear();
        }

        trackerArray.push_back(newValue);

        if(trackerArray.size() > m_numSamplesPerSpan)
            trackerArray.erase(trackerArray.begin());

        m_targetURI.serialize(trackerArray);
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to track \"" << newValue << "\"";
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


float TrackingSpan::average() const
{
    try
    {
        json values = m_averageURI.deserialize();
        float sum = 0;
        for(const json& value : values)
            sum += value.get<float>();

        return sum / values.size();
    }
    catch(json::exception)
    {
        Diagnostics::ExceptionTrace::clear();
    }
    catch(std::runtime_error)
    {
        Diagnostics::ExceptionTrace::clear();
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to average data at \"" << m_averageURI << "\""; 
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
    return NAN;
}


time_t TrackingSpan::getTimeSpanSeconds() const noexcept
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
        json lastSampleJson = m_lastSampleURI.deserialize();
        return lastSampleJson.get<time_t>();
    }
    catch(json::exception)
    {
        Diagnostics::ExceptionTrace::clear();
    }
    catch(std::runtime_error)
    {
        Diagnostics::ExceptionTrace::clear();
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to get timestamp from \"" << m_lastSampleURI << "\"";
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
    return 0;
}


void TrackingSpan::setLastSampleTimestamp(time_t timestamp) const
{
    try
    {
        m_lastSampleURI.serialize(timestamp);
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to set timestamp to \"" << timestamp << "\"";
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


bool TrackingSpan::hasAverageURI() const noexcept
{
    return static_cast<std::string>(m_averageURI).length() > 0;
}


Tracker::Tracker(const Time::Clock& clock, const std::vector<TrackingSpan>& trackingSpans) noexcept : 
    m_clock(clock),
    m_trackingSpans(trackingSpans)
{}


void Tracker::setTrackingSpans(const std::vector<TrackingSpan>& trackingSpans) noexcept
{
    m_trackingSpans = trackingSpans;
}


void Tracker::init()
{
    try
    {
        for(const auto& trackingSpan : m_trackingSpans)
        {
            if(trackingSpan.getLastSampleTimestamp() == 0)
                trackingSpan.setLastSampleTimestamp(m_clock.now());
        }
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to initialize tracker" << std::endl;
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void Tracker::track(float newValue)
{
    try
    {
        if(!isfinite(newValue))
            newValue = 0.0f;

        m_average << newValue;
        time_t now = m_clock.now();

        for(const auto& trackingSpan : m_trackingSpans)
        {
            // Diagnostics::Logger[Level::Debug] << "Handling TrackingSpan: " << trackingSpan.m_targetURI << std::endl;

            time_t secondsPassed = now - trackingSpan.getLastSampleTimestamp();
            time_t secondsBetweenSamples = trackingSpan.getTimeSpanSeconds() / trackingSpan.getNumSamplesPerSpan();
            size_t timesElapsed = secondsPassed / secondsBetweenSamples;
            // Diagnostics::Logger[Level::Debug] << "now: " << now << std::endl;
            // Diagnostics::Logger[Level::Debug] << "lastSample: " << trackingSpan.getLastSampleTimestamp() << std::endl;
            // Diagnostics::Logger[Level::Debug] << "TimesElapsed: " << timesElapsed << std::endl;

            for(size_t i = timesElapsed; i > 0; i--)
            {
                float newValue;
                if(i == 1)
                {
                    if(trackingSpan.hasAverageURI())
                    {
                        newValue = trackingSpan.average();
                        // Diagnostics::Logger[Level::Debug] << "Averaged from json: " << newValue << std::endl;
                    }
                    else
                    {
                        newValue = m_average;
                        m_average.reset();
                        // Diagnostics::Logger[Level::Debug] << "Averaged from 'StreamAverage': " << newValue << " numValues in Average: " << m_average.getNumValues() << std::endl;
                    }
                    trackingSpan.setLastSampleTimestamp(now);
                }
                else
                {
                    newValue = 0.0f;
                }
                trackingSpan.track(newValue);
                // Diagnostics::Logger[Level::Debug] << "Tracked: " << newValue << std::endl;
            }
        }
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to track \"" << newValue << "\"";
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}