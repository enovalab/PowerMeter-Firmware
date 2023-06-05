#include "ACPower.h"
#include <math.h>

using namespace Measuring;

ACPower::ACPower(float voltage_Vrms, float current_Arms, float activePower_W) noexcept : 
    m_voltage_Vrms(voltage_Vrms),
    m_current_Arms(current_Arms),
    m_activePower_W(activePower_W)
{}


float ACPower::getVoltageRms() const noexcept
{
    return m_voltage_Vrms;
}


float ACPower::getCurrentRms() const noexcept
{
    return m_current_Arms;
}


float ACPower::getActivePower() const noexcept
{
    return m_activePower_W;
}


float ACPower::getReactivePower() const noexcept
{
    float apparentPower_VA = getApparentPower();
    float reactivePower_var = sqrt(apparentPower_VA * apparentPower_VA - m_activePower_W * m_activePower_W);
    if(isnanf(reactivePower_var))
        return 0.0f;
    return reactivePower_var;
}


float ACPower::getApparentPower() const noexcept
{
    return m_voltage_Vrms * m_current_Arms;
}


float ACPower::getPowerFactor() const noexcept
{   
    float powerFactor = m_activePower_W / getApparentPower();
    if(powerFactor > 0.99f)
        powerFactor = 1.0f;
    if(isnanf(powerFactor))
        powerFactor = 0.0f;
    return powerFactor;
}