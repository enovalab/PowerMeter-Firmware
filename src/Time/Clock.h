#ifndef CLOCK_H
#define CLOCK_H

#include <time.h>

namespace Time
{
    class Clock
    {
    public:
        virtual time_t now() const = 0;
    };
}

#endif