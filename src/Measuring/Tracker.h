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
            const System::JsonResource& averageResource,
            time_t timeSpanSeconds, 
            size_t numSamplesPerSpan
        );

        float average() const;
        void track(float newValue) const;
        time_t getTimeSpanSeconsds() const;
        time_t getLastSampleTimestamp() const;
        size_t getNumSamplesPerSpan() const;

    private:
        System::JsonResource m_targetResource;
        System::JsonResource m_lastSampleResource;
        System::JsonResource m_averageResource;
        time_t m_timeSpanSeconds;
        size_t m_numSamplesPerSpan;
    };


    class Tracker
    {
    public:
        Tracker(const Time::IClock& clock, const std::vector<TrackingSpan>& configs);
        Tracker& operator<<(float newValue);
    
    private:
        const Time::IClock& m_clock;
        std::vector<TrackingSpan> m_configs;
        StreamAverage<float> m_average;
    };

    void track(const System::JsonResource& targetResource, size_t maxArraySize, float newValue);
    float averageJsonArray(const System::JsonResource&);
}

#endif