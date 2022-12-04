#ifndef TIME_DS3231_H
#define TIME_DS3231_H

#include "IClock.h"
#include <RTClib.h>

namespace Time
{
    class DS3231 : public IClock
    {
    public:
        DS3231();
        time_t now() const override;

    private:
        RTC_DS3231 m_rtc;
    };
}

#endif