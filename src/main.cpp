#if defined(ESP32) && !defined(PIO_UNIT_TESTING)

#include "System/PowerMeter.h"

static bool bootSuccessful = false;

void setup()
{
    bootSuccessful = System::PowerMeter::boot();
}

void loop()
{
    if(bootSuccessful)
        System::PowerMeter::run();
}

#endif