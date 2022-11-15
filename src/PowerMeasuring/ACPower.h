#ifndef ACPOWER_H
#define ACPOWER_H

namespace PowerMeasuring
{
    class ACPower
    {
    public:
        ACPower(float voltageRms, float currentRms, float activePower);
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
    };
}

#endif