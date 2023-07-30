#ifdef ESP32

#include "DS3231.h"
#include <exception>


void DS3231::begin()
{
    if(!m_rtc.begin())
        throw std::runtime_error("Failed to begin I2C Communication to DS3231");
}

time_t DS3231::now() const noexcept
{
    RTC_DS3231 rtc = m_rtc;
    return rtc.now().unixtime();
}

#endif