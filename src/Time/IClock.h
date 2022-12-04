#ifndef TIME_ICLOCK_H
#define TIME_ICLOCK_H

#include <time.h>

namespace Time
{
    class IClock
    {
    public:
        virtual time_t now() const = 0;
        virtual ~IClock();
    };
}

#endif