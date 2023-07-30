#ifndef CLOCK_H
#define CLOCK_H

#include <time.h>

class Clock
{
public:
    virtual time_t now() const noexcept = 0;
    virtual ~Clock() noexcept {};
};

#endif