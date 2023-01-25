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
            const Data::JsonURI& targetURI,
            const Data::JsonURI& lastSampleURI, 
            time_t timeSpanSeconds, 
            size_t numSamplesPerSpan,
            const Data::JsonURI& averageURI = Data::JsonURI()
        );

        float average() const;
        void track(float newValue) const;
        time_t getTimeSpanSeconsds() const noexcept;
        size_t getNumSamplesPerSpan() const noexcept;
        time_t getLastSampleTimestamp() const;
        void setLastSampleTimestamp(time_t timestamp) const;

    private:
        Data::JsonURI m_targetURI;
        Data::JsonURI m_lastSampleURI;
        time_t m_timeSpanSeconds;
        size_t m_numSamplesPerSpan;
        Data::JsonURI m_averageURI;
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