#ifndef MEASURING_TRACKER_H
#define MEASURING_TRACKER_H

#include "Time/IClock.h"
#include "System/JsonResource.h"

#include <StreamAverage.h>
#include <json.hpp>
#include <vector>


namespace Measuring
{
    struct TrackingSpan
    {
        TrackingSpan(
            const System::JsonResource& targetResource,
            const System::JsonResource& lastSampleResource, 
            time_t timeSpanSeconds, 
            size_t numSamplesPerSpan
        ) :
            m_targetResource(targetResource),
            m_lastSampleResource(lastSampleResource),
            m_timeSpanSeconds(timeSpanSeconds),
            m_numSamplesPerSpan(numSamplesPerSpan)
        {}

        System::JsonResource m_targetResource;
        System::JsonResource m_lastSampleResource;
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