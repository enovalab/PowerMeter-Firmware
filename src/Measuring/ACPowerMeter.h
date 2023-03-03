#ifndef MEASURING_ACPOWERMETER_H
#define MEASURING_ACPOWERMETER_H

#include "Measuring/ACPower.h"
#include <stdint.h>
#include <EmonLib.h>


namespace Measuring
{
    class ACPowerMeter
    {
    public:
        ACPowerMeter(uint16_t pinU, uint16_t pinI) noexcept;
        void calibrate(float calU, float calI, float calPhase) noexcept;
        ACPower measure() noexcept;

    private:
        EnergyMonitor emon;
        uint16_t m_pinU;
        uint16_t m_pinI;
    };
}

#endif