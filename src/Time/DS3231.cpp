#ifdef ESP32

#include "DS3231.h"

#include <exception>

using namespace Time;

DS3231::DS3231()
{
    if(!m_rtc.begin())
        throw std::runtime_error("Failed to begin I2C Communication to DS3231");
}

time_t DS3231::now() const
{
    RTC_DS3231 rtc = m_rtc;
    return rtc.now().unixtime();
}

#endif