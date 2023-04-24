#if defined(ESP32) && !defined(PIO_UNIT_TESTING)

#include "Device/PowerMeter.h"

static bool bootSuccessful = false;

void setup()
{
    bootSuccessful = Device::PowerMeter::boot();
}

void loop()
{
    if(bootSuccessful)
        Device::PowerMeter::run();
}

#endif