#ifndef MEASURING_ACPOWERMETER_H
#define MEASURING_ACPOWERMETER_H

#include "Measuring/ACPower.h"
#include <Arduino.h>
#include <vector>

namespace Measuring
{
    class ACPowerMeter
    {
    public:
        ACPowerMeter(uint8_t pinU, uint8_t pinI);
        void calibrate(float calU, float calI, int32_t calPhase);
        ACPower measure(size_t numPeriods = 20);

    private:
        uint8_t m_pinU;
        uint8_t m_pinI;
        float m_calU;
        float m_calI;
        int32_t m_calPhase;
        
        float calculateZero(const std::vector<float>& samples);
    };
}

#endif