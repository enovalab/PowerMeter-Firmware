#include "Tracker.h"

using namespace Measuring;

Tracker& Tracker::operator<<(float newPower)
{
    m_powerAverage << newPower;
    return *this;
}

float Tracker::getEnergy() const
{
    return m_energy;
}

void Tracker::resetEnergy()
{
    m_energy = 0;
}