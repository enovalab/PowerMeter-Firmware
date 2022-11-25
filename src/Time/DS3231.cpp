#include "DS3231.h"

using namespace Time;

DS3231::DS3231()
{
    m_rtc.begin();
}

time_t DS3231::now() const
{
    RTC_DS3231 rtc = m_rtc;
    return rtc.now().unixtime();
}