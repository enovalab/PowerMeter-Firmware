#if defined(ESP32) && !defined(PIO_UNIT_TESTING)

#include <Arduino.h>
#include "Logging/Log.h"
#include "Error/ExceptionStack.h"
#include <json.hpp>


void a()
{
    try
    {
        // json data;
        // data.at("foo");
        throw 21;
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error("a() failed"));
    }
}

void b()
{
    try
    {
        a();
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error("b failed"));
    }
}


void setup()
{
    Serial.begin(115200);

    try
    {
        b();
    }    
    catch(const std::exception& e)
    {
        Logging::Logger[Level::Error] << '\n' << Error::ExceptionStack::what(e) << std::endl;
        // printException(&std::cerr, e);
    }
}

void loop()
{
}

#endif