#ifndef MEASURING_ACPOWER_H
#define MEASURING_ACPOWER_H

namespace Measuring
{
    class ACPower
    {
    public:
        ACPower(float voltageRms, float currentRms, float activePower);
        float getVoltageRms() const;
        float getCurrentRms() const;
        float getActivePower() const;
        float getReactivePower() const;
        float getApparentPower() const;
        float getPowerFactor() const;

    private:
        float m_voltageRms;
        float m_currentRms;
        float m_activePower;
    };
}

#endif