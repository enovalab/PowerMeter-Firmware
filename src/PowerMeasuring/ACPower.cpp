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
    return sqrt(apparentPower * apparentPower + m_activePower * m_activePower);
}


float ACPower::getApparentPower()
{
    return m_voltageRms * m_currentRms;
}


float ACPower::getPowerFactor()
{
    return m_activePower / getApparentPower();
}

