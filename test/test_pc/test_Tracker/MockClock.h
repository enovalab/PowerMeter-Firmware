#ifndef MOCKCLOCK_H
#define MOCKCLOCK_H

#include "Time/IClock.h"

class MockClock : public Time::IClock
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