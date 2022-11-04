#ifndef ACPOWER_H
#define ACPOWER_H

class ACPower
{
public:
    float getVoltageRms();
    float getCurrentRms();
    float getActivePower();
    float getReactivePower();
    float getApparentPower();
    float getPowerFactor();

private:
    float m_voltageRms;
    float m_currentRms;
    float m_activePower;
    friend class PowerMeter;
};

#endif