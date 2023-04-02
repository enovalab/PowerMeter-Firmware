#ifdef ESP32

#include "Measuring/ACPowerMeter.h"
#include "Diagnostics/ExceptionTrace.h"
#include "Diagnostics/Log.h"
#include <EmonLib.h>
#include <sstream>
#include <math.h>


using namespace Measuring;

ACPowerMeter::ACPowerMeter(uint16_t pinU, uint16_t pinI) noexcept : 
    m_pinU(pinU),
    m_pinI(pinI)
{}


void ACPowerMeter::calibrate(float calU, float calI, float calPhase) noexcept
{
    emon.voltage(m_pinU, calU, calPhase);
    emon.current(m_pinI, calI);
}


ACPower ACPowerMeter::measure() noexcept
{
    emon.calcVI(40, 4000);
    // return ACPower(230, 1, 225);
    return ACPower(emon.Vrms, emon.Irms, emon.realPower);
}

#endif