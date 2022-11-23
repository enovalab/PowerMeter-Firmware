#ifndef ACPOWER_H
#define ACPOWER_H

#include <json.hpp>

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
        operator json() const;

    private:
        float m_voltageRms;
        float m_currentRms;
        float m_activePower;
    };
}

#endif