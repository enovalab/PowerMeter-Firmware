#include "PowerMeter.h"
#include "StreamRMS.h"
#include "StreamActivePower.h"
#include <Ewma.h>

PowerMeter::PowerMeter(uint8_t pinU, uint8_t pinI) : 
    m_pinU(pinU),
    m_pinI(pinI)
{}

void PowerMeter::calibrate(float calU, float calI, int32_t calPhase)
{
    m_calU = calU;
    m_calI = calI;
    m_calPhase = calPhase;
}


ACPower PowerMeter::measure(size_t numPeriods)
{
    // Puffer deklarieren
    std::vector<float> samplesU;
    std::vector<float> samplesI;

    // Für n Periodendauern Messwerte in Puffer speichern
    uint32_t startMicros = micros();
    while(micros() - startMicros < 20000 * numPeriods)
    {
        samplesU.push_back(analogRead(m_pinU));
        samplesI.push_back(analogRead(m_pinI));
    }

    // DC Offset berechnen (arith. Mittelwert)
    float zeroU = calcZero(samplesU);
    float zeroI = calcZero(samplesI);

    // Messwerte filtern und Kalibrieren
    Ewma ewmaI(0.05, 0.0);
    StreamRMS<float> streamU;
    StreamRMS<float> streamI;
    StreamActivePower<float> streamP;
    
    for(size_t i = 0; i < samplesU.size(); i++)
    {
        float instantU = (samplesU[i] - zeroU) * m_calU;
        float instantI = ewmaI.filter((samplesI[makeIndexCircular(i + m_calPhase, samplesI.size())] - zeroI) * m_calI);
        streamU << instantU;
        streamI << instantI;
        streamP << instantU * instantI;
    }

    return ACPower(streamU, streamI, streamP);
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
    
    return sqrt(sum / samples.size());
}


float PowerMeter::calcActivePower(const std::vector<float>& samplesU, const std::vector<float>& samplesI)
{
    float sum = 0.0f;

    for(size_t i = 0; i < samplesU.size(); i++)
        sum += samplesU[i] * samplesI[i];
    
    return sum / samplesU.size();
}


size_t PowerMeter::makeIndexCircular(int32_t index, size_t size)
{
	index %= static_cast<int32_t>(size);
	if (index < 0)
		return size + index;
	
	return index;
}