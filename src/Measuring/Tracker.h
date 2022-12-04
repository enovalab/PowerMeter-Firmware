#ifndef MEASURING_TRACKER_H
#define MEASURING_TRACKER_H

#include "Time/IClock.h"
#include "System/JsonResource.h"

#include <StreamAverage.h>
#include <json.hpp>
#include <string>
#include <vector>
#include <fstream>

namespace Measuring
{
    class TrackingSpan
    {
    public:
        TrackingSpan(
            const System::JsonResource& jsonResource, 
            time_t timeSpanSeconds, 
            size_t numSamplesPerSpan, 
            time_t lastSampleTimestamp
        );

        System::JsonResource getTargetResource() const;
        time_t getTimeSpanSeconds() const;
        size_t getNumSamplesPerSpan() const;
        time_t getLastSampleTimestamp() const;
        void pushBackPopFront(float newValue) const;

    private:
        System::JsonResource m_targetResource;
        time_t m_timeSpanSeconds;
        size_t m_numSamplesPerSpan;
        time_t m_lastSampleTimestamp;
    };


    class Tracker
    {
    public:
        Tracker(const Time::IClock& clock, const std::vector<TrackingSpan>& configs);
        Tracker& operator<<(float newValue);
    
    private:
        static float getJsonArrayAverage(const System::JsonResource& jsonResource);

        const Time::IClock& m_clock;
        std::vector<TrackingSpan> m_configs;
        StreamAverage<float> m_average;
    };
}

#endif