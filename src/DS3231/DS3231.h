#ifndef DS3231_H
#define DS3231_H

#include "Clock/Clock.h"
#include <RTClib.h>


class DS3231 : public Clock
{
public:
    void begin();
    time_t now() const noexcept override;

private:
    RTC_DS3231 m_rtc;
};

#endif