#ifdef ESP32

#include "Measuring/ACPowerMeter.h"
#include "Measuring/StreamRMS.h"
#include "Measuring/StreamActivePower.h"
#include <Ewma.h>

using namespace Measuring;

namespace
{
    size_t makeIndexCircular(int32_t index, size_t size) noexcept
    {
        index %= static_cast<int32_t>(size);
        if (index < 0)
            return size + index;
        
        return index;
    }

    float calculateZero(const std::vector<float>& samples) noexcept
    {
        float sum;
        for(const auto& sample : samples)
            sum += sample;
        return sum / samples.size();
    }
}


ACPowerMeter::ACPowerMeter(uint8_t pinU, uint8_t pinI) noexcept : 
    m_pinU(pinU),
    m_pinI(pinI)
{}


void ACPowerMeter::calibrate(float calU, float calI, int32_t calPhase) noexcept
{
    m_calU = calU;
    m_calI = calI;
    m_calPhase = calPhase;
}

Measuring::ACPower ACPowerMeter::measure(size_t numPeriods) noexcept
{
    std::vector<float> samplesU;
    std::vector<float> samplesI;

    uint32_t startMicros = micros();
    while(micros() - startMicros < 20000 * numPeriods)
    {
        samplesU.push_back(analogRead(m_pinU));
        samplesI.push_back(analogRead(m_pinI));
    }

    float zeroU = calculateZero(samplesU);
    float zeroI = calculateZero(samplesI);

    Ewma ewmaI(0.12, 0);
    Measuring::StreamRMS<float> streamU;
    Measuring::StreamRMS<float> streamI;
    Measuring::StreamActivePower<float> streamP;
    
    for(size_t i = 0; i < samplesU.size(); i++)
    {
        float instantU = (samplesU[i] - zeroU) * m_calU;
        float instantI = ewmaI.filter((samplesI[makeIndexCircular(i + m_calPhase, samplesI.size())] - zeroI) * m_calI);
        streamU << instantU;
        streamI << instantI;
        streamP << instantU * instantI;
    }

    return Measuring::ACPower(
        (streamU < 5.0f) ? 0.0f : streamU,
        (streamI < 0.04f) ? 0.0f : streamI,
        (streamP < 1.0f) ? 0.0f : streamP
    );
}

#endif