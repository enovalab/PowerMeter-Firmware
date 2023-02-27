#ifndef MEASURING_ACPOWERMETER_H
#define MEASURING_ACPOWERMETER_H

#include "Measuring/ACPower.h"
#include <driver/adc.h>
#include <Arduino.h>


namespace Measuring
{
    class ACPowerMeter
    {
    public:
        ACPowerMeter(uint16_t pinU, uint16_t pinI);
        void calibrate(float calU, float calI, float calPhase) noexcept;
        ACPower measure(size_t numSamples = 20);

    private:
        adc1_channel_t m_adcChannelU = ADC1_CHANNEL_0;
        adc1_channel_t m_adcChannelI = ADC1_CHANNEL_1;
        float m_calU = 0.0f;
        float m_calI = 0.0f;
        float m_calPhase = 0.0f;
    };
}

#endif