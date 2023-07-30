#ifdef ESP32

#include "ACPowerMeter.h"
#include <EmonLib.h>
#include <math.h>

void ACPowerMeter::setPins(uint16_t pinU, uint16_t pinI) noexcept
{
    m_pinU = pinU;
    m_pinI = pinI;
}


void ACPowerMeter::calibrate(float calU, float calI, float calPhase) noexcept
{
    emon.voltage(m_pinU, calU, calPhase);
    emon.current(m_pinI, calI);
}


ACPower ACPowerMeter::measure() const noexcept
{

    // emon.calcVI(40, 4000);
    // return ACPower(emon.Vrms, emon.Irms, emon.realPower);
    delay(400);
    return ACPower(230, 1.5, rand());
}

#endif