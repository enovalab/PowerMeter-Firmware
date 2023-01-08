#include "ErrorHandling/NestedException.h"

#include <functional>
#include <sstream>

using namespace ErrorHandling;


static void rethrowNestedRecursive(const std::nested_exception& exception)
{
    try
    {
        exception.rethrow_nested();
    }
    catch(const std::nested_exception& nestedException)
    {
        rethrowNestedRecursive(nestedException);
    }
}


void NestedException::rethrowMostNested()
{
    try
    {
        throw;
    }
    catch(const std::nested_exception& nestedException)
    {
        rethrowNestedRecursive(nestedException);
    } 
}


std::string NestedException::what(uint8_t indentLevel, char indentChar)
{
    std::stringstream what;
    try
    {
        throw;
    }
    catch(const std::exception& exception)
    {
        uint8_t nestingLevel = 0;
        std::function<void(const std::exception&)> appendToWhat = [&](const std::exception& exception) {
            what << std::string(static_cast<size_t>(nestingLevel), indentChar) << exception.what() << '\n';
            nestingLevel += indentLevel;
            try
            {
                std::rethrow_if_nested(exception);
            }
            catch (const std::exception& nestedException)
            {
                appendToWhat(nestedException);
            }
            catch(...)
            {}
        };
        appendToWhat(exception);
    }

    return what.str();
}