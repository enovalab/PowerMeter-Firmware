#ifndef POWERMETER_H
#define POWERMETER_H

#include "ACPower.h"
#include <Arduino.h>
#include <vector>

class PowerMeter
{
public:
    PowerMeter(uint8_t pinU, uint8_t pinI);
    void calibrate(float calU, float calI, int32_t calPhase);
    ACPower measure(size_t numPeriods = 5);

private:
    uint8_t m_pinU;
    uint8_t m_pinI;
    float m_calU;
    float m_calI;
    int32_t m_calPhase;
    
    float calcZero(const std::vector<float>& samples);
    float calcRms(const std::vector<float>& samples);
    float calcActivePower(const std::vector<float>& samplesU, const std::vector<float>& samplesI);
    size_t makeIndexCircular(int32_t index, size_t size);
};

#endif