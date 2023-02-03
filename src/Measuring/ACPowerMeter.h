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
        ACPowerMeter(uint8_t pinU, uint8_t pinI) noexcept;
        void calibrate(float calU, float calI, int32_t calPhase) noexcept;
        ACPower measure(size_t numPeriods = 20) noexcept;

    private:
        uint8_t m_pinU = 0;
        uint8_t m_pinI = 0;
        float m_calU = 0.0f;
        float m_calI = 0.0f;
        int32_t m_calPhase = 0;
    };
}

#endif