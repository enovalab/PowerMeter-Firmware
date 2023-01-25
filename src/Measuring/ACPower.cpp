#include "ACPower.h"
#include <math.h>

using namespace Measuring;

ACPower::ACPower(float voltageRms, float currentRms, float activePower) : 
    m_voltageRms(voltageRms),
    m_currentRms(currentRms),
    m_activePower(activePower)
{}


float ACPower::getVoltageRms() const
{
    return m_voltageRms;
}


float ACPower::getCurrentRms() const
{
    return m_currentRms;
}


float ACPower::getActivePower() const
{
    return m_activePower;
}


float ACPower::getReactivePower() const
{
    float apparentPower = getApparentPower();
    float activePower = getPowerFactor() * apparentPower;
    float reactivePower = sqrt(apparentPower * apparentPower - activePower * activePower);
    if(isnanf(reactivePower))
        return 0.0f;
    return reactivePower;
}


float ACPower::getApparentPower() const
{
    return m_voltageRms * m_currentRms;
}


float ACPower::getPowerFactor() const
{   
    float powerFactor = m_activePower / getApparentPower();
    if(powerFactor > 0.99f)
        powerFactor = 1.0f;
    return powerFactor;
}