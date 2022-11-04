#include <PowerMeter.h>
#include <Ewma.h>

PowerMeter::PowerMeter(uint8_t pinU, uint8_t pinI) : 
    m_pinU(pinU),
    m_pinI(pinI)
{}

void PowerMeter::calibrate(float calU, float calI, size_t calPhi)
{
    m_calU = calU;
    m_calI = calI;
    m_calPhi = calPhi;
}


ACPower PowerMeter::measure(size_t numPeriods = 5)
{
    Ewma ewmaI(0.05, 0.0);
    std::vector<float> samplesU;
    std::vector<float> samplesI;
    samplesU.reserve(100 * numPeriods);
    samplesI.reserve(100 * numPeriods);

    uint32_t startMicros = micros();
    while(micros() - startMicros < 20000 * numPeriods)
    {
        samplesU.push_back(analogRead(m_pinU) * m_calU);
        samplesI.push_back(analogRead(m_pinI) * m_calI);
    }

    float zeroU = calcZero(samplesU);
    float zeroI = calcZero(samplesI);
  
}

float PowerMeter::calcZero(const std::vector<float>& samples)
{
    float sum;
    for(const auto& sample : samples)
        sum += sample;
    return sum / samples.size();
}


float PowerMeter::calcRms(const std::vector<float>& samples)
{
    float sum = 0.0f;

    for (const auto& sample : samples)
        sum += sample * sample;
    
    return sqrt(sum / static_cast<float>(samples.size()));
}


float PowerMeter::calcActivePower(const std::vector<float>& samplesU, const std::vector<float>& samplesI)
{

}


