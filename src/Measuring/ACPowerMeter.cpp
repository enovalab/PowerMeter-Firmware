#ifdef ESP32

#include "Measuring/ACPowerMeter.h"
#include "Measuring/StreamRMS.h"
#include "Diagnostics/ExceptionTrace.h"
#include "Diagnostics/Log.h"
#include <StreamAverage.h>
#include <Ewma.h>
#include <iostream>
#include <sstream>
#include <array>
#include <vector>
#include <math.h>

using namespace Measuring;

namespace
{
    adc1_channel_t gpioToAdc1Channel(uint16_t pin)
    {
        for(adc1_channel_t channel; channel < ADC1_CHANNEL_MAX; channel = static_cast<adc1_channel_t>(channel + 1))
        {                
            gpio_num_t gpio;
            adc1_pad_get_io_num(channel, &gpio);
            if(static_cast<uint16_t>(gpio) == pin)
                return channel;
        }
        std::stringstream errorMessage;
        errorMessage << pin << " is not a Channel of ADC1";
        throw std::runtime_error(errorMessage.str());
    }
    
    float compensatePhaseShift(float* sampleBuffer, size_t index, float calPhase)
    {
        auto at = [sampleBuffer](int32_t index){
            size_t numSamples = sizeof(sampleBuffer) / sizeof(float);
            index %= static_cast<int32_t>(numSamples);
            if (index < 0)
                index += numSamples;
            return sampleBuffer[index];
        };
        
        index += static_cast<int32_t>(calPhase);
        size_t indexNext = index + 1 ;
        float weightA = calPhase - static_cast<int32_t>(calPhase);
        if(weightA == 0)
            return at(index);
        float weightB = 1 - weightA;
        float valueA = at(index);
        float valueB = at(indexNext);
        return valueA * weightB + valueB * weightA;
    }

    template<typename T>
    T average(T* sampleBuffer) noexcept
    {
        size_t numSamples = sizeof(sampleBuffer) / sizeof(T);
        T sum;
        for(size_t i = 0; i < numSamples; i++)
            sum += sampleBuffer[i];
        return sum / static_cast<float>(numSamples);
    }
}


ACPowerMeter::ACPowerMeter(uint16_t pinU, uint16_t pinI)
{
    try
    {
        m_adcChannelU = gpioToAdc1Channel(pinU);
        m_adcChannelI = gpioToAdc1Channel(pinI);
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to construct from pinU = " << pinU << ", pinI = " << pinI;
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void ACPowerMeter::calibrate(float calU, float calI, float calPhase) noexcept
{
    m_calU = calU;
    m_calI = calI;
    m_calPhase = calPhase;
}

ACPower ACPowerMeter::measure()
{
    constexpr size_t numPeriods = 20;

    std::array<uint16_t, numPeriods * 300> samplesU;
    std::array<uint16_t, numPeriods * 300> samplesI;

    size_t bufferIndex = 0;
    uint32_t startMicros = micros();
    while(micros() - startMicros < (numPeriods * 20000))
    {
        samplesU[bufferIndex] = adc1_get_raw(m_adcChannelU);
        samplesI[bufferIndex] = adc1_get_raw(m_adcChannelI);
        bufferIndex++;
    }

    uint16_t zeroU = average(samplesU.data());
    uint16_t zeroI = average(samplesI.data());

    for(size_t i = 0; i < bufferIndex; i++)
    {
        float instantU = samplesU[i] - zeroU;
        float instantI = samplesI[i] - zeroI;
        std::cout << instantU << std::endl;
    }

    for(size_t i = 0; i < 100; i++)
    {
        std::cout << 0 << std::endl;
    }

    return ACPower(0, 0, 0);
}

#endif