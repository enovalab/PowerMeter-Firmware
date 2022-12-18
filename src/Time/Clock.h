#ifndef TIME_CLOCK_H
#define TIME_CLOCK_H

#include <time.h>

namespace Time
{
    class Clock
    {
    public:
        virtual time_t now() const = 0;
        virtual ~Clock() {};
    };
}

#endif