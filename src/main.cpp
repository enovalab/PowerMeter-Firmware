#include <Arduino.h>
#include "Logging/Log.h"
#include "ErrorManagement/ExceptionStack.h"
#include <json.hpp>

void printException(std::ostream* stream, const std::exception& e, int level = 0, char indentChar = '\t')
{
    (*stream) << std::string(level, indentChar) << e.what() << '\n';
    try 
    {
        std::rethrow_if_nested(e);
    }
    catch (const std::exception& nestedException)
    {
        printException(stream, nestedException, level + 1, indentChar);
    }
    catch (...)
    {
        for (size_t i = 0; i <= level; i++)
            (*stream) << indentChar;
        (*stream) << "Unexpected Exception occurred";
    }
}

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
        Logging::Logger[Logging::Level::Error] << '\n' << ErrorManagement::ExceptionStack::what(e) << std::endl;
        // printException(&std::cerr, e);
    }
}

void loop()
{
}