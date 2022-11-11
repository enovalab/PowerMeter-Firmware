#include "ACPower.h"
#include <math.h>

ACPower::ACPower(float voltageRms, float currentRms, float activePower) : 
    m_voltageRms(voltageRms),
    m_currentRms(currentRms),
    m_activePower(activePower)
{}

float ACPower::getVoltageRms()
{
    return m_voltageRms;
}


float ACPower::getCurrentRms()
{
    return m_currentRms;
}


float ACPower::getActivePower()
{
    return m_activePower;
}


float ACPower::getReactivePower()
{
    float apparentPower = getApparentPower();
    float activePower = getPowerFactor() * apparentPower;
    float reactivePower = sqrt(apparentPower * apparentPower - activePower * activePower);
    if(isnanf(reactivePower))
        return 0.0f;
    return reactivePower;
}


float ACPower::getApparentPower()
{
    return m_voltageRms * m_currentRms;
}


float ACPower::getPowerFactor()
{   
    float powerFactor = m_activePower / getApparentPower();
    if(powerFactor > 0.99f)
        powerFactor = 1.0f;
    return powerFactor;
}

