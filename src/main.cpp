#if defined(ESP32) && !defined(PIO_UNIT_TESTING)

#include "System/PowerMeter.h"

void setup()
{
    System::PowerMeter::init();
}

void loop()
{
    System::PowerMeter::run();
}

#endif