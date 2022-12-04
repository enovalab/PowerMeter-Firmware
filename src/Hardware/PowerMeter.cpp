#include "PowerMeter.h"
#include "Measuring/StreamRMS.h"
#include "Measuring/StreamActivePower.h"
#include <Ewma.h>

using namespace Hardware;

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


Measuring::ACPower PowerMeter::measure(size_t numPeriods)
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

    // Messwerte filtern, kalibrieren und in Stream schieben
    Ewma ewmaI(0.12, 0);
    Measuring::StreamRMS<float> streamU;
    Measuring::StreamRMS<float> streamI;
    Measuring::StreamActivePower<float> streamP;
    
    for(size_t i = 0; i < samplesU.size(); i++)
    {
        float instantU = (samplesU[i] - zeroU) * m_calU;
        float instantI = ewmaI.filter((samplesI[makeIndexCircular(i + m_calPhase, samplesI.size())] - zeroI) * m_calI);
        streamU << instantU;
        streamI << instantI;
        streamP << instantU * instantI;
    }
    return Measuring::ACPower(streamU, streamI, streamP);
}


float PowerMeter::calcZero(const std::vector<float>& samples)
{
    float sum;
    for(const auto& sample : samples)
        sum += sample;
    return sum / samples.size();
}


size_t PowerMeter::makeIndexCircular(int32_t index, size_t size)
{
	index %= static_cast<int32_t>(size);
	if (index < 0)
		return size + index;
	
	return index;
}