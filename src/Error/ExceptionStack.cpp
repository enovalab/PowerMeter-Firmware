#include "Error/ExceptionStack.h"

#include <sstream>

using namespace Error;


ExceptionStack_t ExceptionStack::get()
{
    ExceptionStack_t exceptionStack;
    try
    {
        throw;
    }
    catch(const std::exception& exception)
    {
        std::function<void(const std::exception&)> storeException = [&](const std::exception& exception) {
            exceptionStack.push_back(std::current_exception());
            try
            {
                std::rethrow_if_nested(exception);
            }
            catch (const std::exception& nestedError)
            {
                storeException(nestedError);
            }
            catch(...)
            {}
        };
        storeException(exception);
    }   
    return exceptionStack;
}


ExceptionStack_t ExceptionStack::get(const std::exception& exception)
{
    ExceptionStack_t exceptionStack;
    std::function<void(const std::exception&)> storeException = [&](const std::exception& exception) {
        exceptionStack.push_back(std::current_exception());
        try
        {
            std::rethrow_if_nested(exception);
        }
        catch (const std::exception& nestedError)
        {
            storeException(nestedError);
        }
        catch(...)
        {}
    };
    storeException(exception);
    return exceptionStack;
}


std::string ExceptionStack::what(const std::exception& exception, uint8_t indent, char indentChar)
{
    return what(get(exception), indent, indentChar);
}


std::string ExceptionStack::what(const ExceptionStack_t& exceptionStack, uint8_t indent, char indentChar)
{
    std::stringstream errorMessage;
    for (size_t nestingLevel = 0; nestingLevel < exceptionStack.size(); nestingLevel++)
    {
        for(size_t indentLevel = 0; indentLevel < indent * nestingLevel; indentLevel++)
            errorMessage << indentChar;

        try
        {
            std::rethrow_exception(exceptionStack[nestingLevel]);
        }
        catch (const std::exception& exception)
        {
            errorMessage << exception.what() << '\n';
        }
        catch (...)
        {
            errorMessage << "Unexpected" << '\n';
        }
    }
    return errorMessage.str();
}

