#ifndef MOCKCLOCK_H
#define MOCKCLOCK_H

#include "Time/Clock.h"

class MockClock : public Time::Clock
{
public:    
    void setTime(time_t now)
    {
        m_now = now;
    }

    time_t now() const override
    {
        return m_now;
    }
private:
    time_t m_now = 0;
};

#endif