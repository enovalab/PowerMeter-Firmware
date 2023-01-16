#ifndef DATA_TRACKER_H
#define DATA_TRACKER_H

#include "Time/Clock.h"
#include "Data/JsonURI.h"

#include <StreamAverage.h>
#include <json.hpp>
#include <vector>


namespace Data
{
    class TrackingSpan
    {
    public:
        TrackingSpan(
            const Data::JsonURI& targetResource,
            const Data::JsonURI& lastSampleResource, 
            time_t timeSpanSeconds, 
            size_t numSamplesPerSpan,
            const Data::JsonURI& averageResource = Data::JsonURI()
        );

        float average() const;
        void track(float newValue) const;
        time_t getTimeSpanSeconsds() const noexcept;
        size_t getNumSamplesPerSpan() const noexcept;
        time_t getLastSampleTimestamp() const;
        void setLastSampleTimestamp(time_t timestamp) const;

    private:
        Data::JsonURI m_targetResource;
        Data::JsonURI m_lastSampleResource;
        time_t m_timeSpanSeconds;
        size_t m_numSamplesPerSpan;
        Data::JsonURI m_averageResource;
    };


    class Tracker
    {
    public:
        Tracker(const Time::Clock& clock, const std::vector<TrackingSpan>& trackingSpans);
        void track(float newValue);
    
    private:
        const Time::Clock& m_clock;
        std::vector<TrackingSpan> m_trackingSpans;
        StreamAverage<float> m_average;
    };
}

#endif