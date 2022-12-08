#ifndef MEASURING_TRACKER_H
#define MEASURING_TRACKER_H

#include "Time/IClock.h"
#include "System/JsonResource.h"

#include <StreamAverage.h>
#include <json.hpp>
#include <vector>


namespace Measuring
{
    class TrackingSpan
    {
    public:
        TrackingSpan(
            const System::JsonResource& targetResource,
            const System::JsonResource& lastSampleResource, 
            time_t timeSpanSeconds, 
            size_t numSamplesPerSpan,
            const System::JsonResource& averageResource = System::JsonResource()
        );

        float average() const;
        void track(float newValue) const;
        time_t getTimeSpanSeconsds() const;
        size_t getNumSamplesPerSpan() const;
        time_t getLastSampleTimestamp() const;
        void setLastSampleTimestamp(time_t timestamp) const;

    private:
        System::JsonResource m_targetResource;
        System::JsonResource m_lastSampleResource;
        time_t m_timeSpanSeconds;
        size_t m_numSamplesPerSpan;
        System::JsonResource m_averageResource;
    };


    class Tracker
    {
    public:
        Tracker(const Time::IClock& clock, const std::vector<TrackingSpan>& configs);
        void track(float newValue);
    
    private:
        const Time::IClock& m_clock;
        std::vector<TrackingSpan> m_configs;
        StreamAverage<float> m_average;
    };

    void track(const System::JsonResource& targetResource, size_t maxArraySize, float newValue);
    float averageJsonArray(const System::JsonResource&);
}

#endif