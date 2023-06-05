#ifndef MEASURING_ACPOWER_H
#define MEASURING_ACPOWER_H

namespace Measuring
{
    class ACPower
    {
    public:
        ACPower(float voltage_Vrms, float current_Arms, float activePower_W) noexcept;
        float getVoltageRms() const noexcept;
        float getCurrentRms() const noexcept;
        float getActivePower() const noexcept;
        float getReactivePower() const noexcept;
        float getApparentPower() const noexcept;
        float getPowerFactor() const noexcept;

    private:
        float m_voltage_Vrms;
        float m_current_Arms;
        float m_activePower_W;
    };
}

#endif