#ifndef MOCKCLOCK_H
#define MOCKCLOCK_H

#include "Time/Clock.h"

namespace Mock
{
    class MockClock : public Time::Clock
    {
    public:
        MockClock(time_t now) : m_now(now)
        {}

        time_t now() const override
        {
            return m_now;
        }
    private:
        time_t m_now;
    };
}

#endif