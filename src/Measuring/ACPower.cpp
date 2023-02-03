#include "ACPower.h"
#include <math.h>

using namespace Measuring;

ACPower::ACPower(float voltageRms, float currentRms, float activePower) noexcept : 
    m_voltageRms(voltageRms),
    m_currentRms(currentRms),
    m_activePower(activePower)
{}


float ACPower::getVoltageRms() const noexcept
{
    return m_voltageRms;
}


float ACPower::getCurrentRms() const noexcept
{
    return m_currentRms;
}


float ACPower::getActivePower() const noexcept
{
    return m_activePower;
}


float ACPower::getReactivePower() const noexcept
{
    float apparentPower = getApparentPower();
    float activePower = getPowerFactor() * apparentPower;
    float reactivePower = sqrt(apparentPower * apparentPower - activePower * activePower);
    if(isnanf(reactivePower))
        return 0.0f;
    return reactivePower;
}


float ACPower::getApparentPower() const noexcept
{
    return m_voltageRms * m_currentRms;
}


float ACPower::getPowerFactor() const noexcept
{   
    float powerFactor = m_activePower / getApparentPower();
    if(powerFactor > 0.99f)
        powerFactor = 1.0f;
    if(isnanf(powerFactor))
        powerFactor = 0.0f;
    return powerFactor;
}