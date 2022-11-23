#ifndef TRACKER_H
#define TRACKER_H

#include <StreamAverage.h>

namespace Measuring
{
    class Tracker
    {
    public:
        Tracker& operator<<(float newPower);
        float getEnergy() const;
        void resetEnergy();
    
    private:
        float m_energy = 0;
        StreamAverage<float> m_powerAverage;
    };
}

#endif