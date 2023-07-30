#ifndef ACPOWER_H
#define ACPOWER_H

class ACPower
{
public:
    ACPower(float voltageRMS_V, float currentRMS_A, float activePower_W) noexcept;
    float getVoltageRMS_V() const noexcept;
    float getCurrentRMS_A() const noexcept;
    float getActivePower_W() const noexcept;
    float getReactivePower_var() const noexcept;
    float getApparentPower_VA() const noexcept;
    float getPowerFactor() const noexcept;

private:
    float m_voltageRMS_V;
    float m_currentRMS_A;
    float m_activePower_W;
};

#endif