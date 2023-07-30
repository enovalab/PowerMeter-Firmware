#include "ACPower.h"
#include <math.h>

ACPower::ACPower(float voltageRMS_V, float currentRMS_A, float activePower_W) noexcept : 
    m_voltageRMS_V(voltageRMS_V),
    m_currentRMS_A(currentRMS_A),
    m_activePower_W(activePower_W)
{}


float ACPower::getVoltageRMS_V() const noexcept
{
    return m_voltageRMS_V;
}


float ACPower::getCurrentRMS_A() const noexcept
{
    return m_currentRMS_A;
}


float ACPower::getActivePower_W() const noexcept
{
    return m_activePower_W;
}


float ACPower::getReactivePower_var() const noexcept
{
    float apparentPower_VA = getApparentPower_VA();
    float reactivePower_var = sqrt(apparentPower_VA * apparentPower_VA - m_activePower_W * m_activePower_W);
    if(isnanf(reactivePower_var))
        return 0.0f;
    return reactivePower_var;
}


float ACPower::getApparentPower_VA() const noexcept
{
    return m_voltageRMS_V * m_currentRMS_A;
}


float ACPower::getPowerFactor() const noexcept
{   
    float powerFactor = m_activePower_W / getApparentPower_VA();
    if(powerFactor > 0.99f)
        powerFactor = 1.0f;
    if(isnanf(powerFactor))
        powerFactor = 0.0f;
    return powerFactor;
}