#ifndef ACPOWERMETER_H
#define ACPOWERMETER_H

#include "ACPower/ACPower.h"
#include <stdint.h>
#include <EmonLib.h>


class ACPowerMeter
{
public:
    void setPins(uint16_t pinU, uint16_t pinI) noexcept;
    void calibrate(float calU, float calI, float calPhase) noexcept;
    ACPower measure() const noexcept;

private:
    EnergyMonitor emon;
    uint16_t m_pinU = 0;
    uint16_t m_pinI = 0;
};


#endif