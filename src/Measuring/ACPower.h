#ifndef MEASURING_ACPOWER_H
#define MEASURING_ACPOWER_H

namespace Measuring
{
    class ACPower
    {
    public:
        ACPower(float voltage_Vrms, float current_Arms, float activePower_W) noexcept;
        float getVoltage_Vrms() const noexcept;
        float getCurrent_Arms() const noexcept;
        float getActivePower_W() const noexcept;
        float getReactivePower_var() const noexcept;
        float getApparentPower_VA() const noexcept;
        float getPowerFactor() const noexcept;

    private:
        float m_voltage_Vrms;
        float m_current_Arms;
        float m_activePower_W;
    };
}

#endif