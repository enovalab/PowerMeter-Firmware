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
    
    float compensatePhaseShift(const float* sampleBuffer, size_t index, float calPhase)
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

    class ExponetialHighPassFilter
    {
    private: 
        float m_beta;
        float m_previousInput;
        float m_previousOutput;

    public:
        ExponetialHighPassFilter(float beta, float initInput, float initOutput) : 
            m_beta(beta),
            m_previousInput(initInput),
            m_previousOutput(initOutput)
        {}

        float operator()(float input)
        {
            float output = 0.5f * (2 - m_beta) * (input - m_previousInput) + (1 - m_beta) * m_previousOutput;
            m_previousInput = input;
            m_previousOutput = output;
            return output;
        }
    };
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

// ACPower ACPowerMeter::measure()
// {
//     constexpr size_t numPeriods = 20;

//     std::vector<float> samplesU;
//     std::vector<float> samplesI;

//     uint32_t startMicros = micros();
//     while(micros() - startMicros < (numPeriods * 20000))
//     {
//         samplesU.push_back(adc1_get_raw(m_adcChannelU));
//         samplesI.push_back(adc1_get_raw(m_adcChannelI));
//     }

//     float sumU = 0.0f;
//     float sumI = 0.0f;
//     float sumP = 0.0f;

//     ExponetialHighPassFilter filterU(0.01f, samplesU[0] * m_calU, 0);
//     ExponetialHighPassFilter filterI(0.01f, samplesI[0] * m_calI, 0);

//     for(size_t i = 0; i < samplesU.size(); i++)
//     {
//         float instantU = filterU(samplesU[i] * m_calU);
//         float instantI = filterI(samplesI[i] * m_calI);
//         float instantP = instantU * instantI;

//         sumU += instantU * instantU;
//         sumI += instantI * instantI;
//         sumP += instantP;
//     }

//     float rmsU = sqrtf(sumU / samplesU.size());
//     float rmsI = sqrtf(sumI / samplesI.size());
//     float avgP = sumP / samplesU.size();

//     std::cout << "Sample Rate: " << samplesU.size() / (numPeriods * 20.0f) << " kHz, Free Heap: " << ESP.getFreeHeap() << " bytes" << std::endl; 

//     return ACPower(
//         (rmsU < 5.0f) ? 0.0f : rmsU,
//         (rmsI < 0.04f) ? 0.0f : rmsI,
//         (avgP < 1.0f) ? 0.0f : avgP
//     );
// }

ACPower ACPowerMeter::measure(size_t numPeriods)
{
    ExponetialHighPassFilter filterU(0.01f, adc1_get_raw(m_adcChannelU) * m_calU, 0.0f);
    ExponetialHighPassFilter filterI(0.01f, adc1_get_raw(m_adcChannelI) * m_calI, 0.0f);
    StreamRMS<float> streamU;
    StreamRMS<float> streamI;
    StreamAverage<float> streamP;

    size_t numSamples = 0;
    uint32_t startMicros = micros();
    while(micros() - startMicros < (numPeriods * 20000))
    {
        numSamples++;
        float instantU = filterU(adc1_get_raw(m_adcChannelU) * m_calU);
        float instantI = filterI(adc1_get_raw(m_adcChannelI) * m_calI);
        float instantP = instantU * instantI;

        streamU << instantU;
        streamI << instantI;
        streamP << instantP;
    } 

    return ACPower(
        (streamU.get() < 5.0f) ? 0.0f : streamU.get(),
        (streamI.get() < 0.04f) ? 0.0f : streamI,
        (streamP < 1.0f) ? 0.0f : streamP
    );
}

#endif