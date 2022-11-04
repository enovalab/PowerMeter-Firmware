#ifndef POWERMETER_H
#define POWERMETER_H

#include <ACPower.h>
#include <Arduino.h>
#include <vector>

class PowerMeter
{
public:
    PowerMeter(uint8_t pinU, uint8_t pinI);
    void calibrate(float calU, float calI, size_t calPhi);
    ACPower measure(size_t numPeriods = 5);

private:
    uint8_t m_pinU;
    uint8_t m_pinI;
    float m_calU;
    float m_calI;
    size_t m_calPhi;

    float calcZero(const std::vector<float>& samples);
    float calcRms(const std::vector<float>& samples);
    float calcActivePower(const std::vector<float>& samplesU, const std::vector<float>& samplesI);
};

#endif