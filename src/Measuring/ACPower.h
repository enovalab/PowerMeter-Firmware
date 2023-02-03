#ifndef MEASURING_ACPOWER_H
#define MEASURING_ACPOWER_H

namespace Measuring
{
    class ACPower
    {
    public:
        ACPower(float voltageRms, float currentRms, float activePower) noexcept;
        float getVoltageRms() const noexcept;
        float getCurrentRms() const noexcept;
        float getActivePower() const noexcept;
        float getReactivePower() const noexcept;
        float getApparentPower() const noexcept;
        float getPowerFactor() const noexcept;

    private:
        float m_voltageRms;
        float m_currentRms;
        float m_activePower;
    };
}

#endif