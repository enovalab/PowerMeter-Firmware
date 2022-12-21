#include "ErrorManagement/ExceptionStack.h"


#include <sstream>
#include <functional>

using namespace ErrorManagement;

ExceptionStack::ExceptionStack_t ExceptionStack::get(const std::exception& exception)
{
    ExceptionStack_t exceptionStack;
    std::function<void(const std::exception&)> storeException = [&](const std::exception& exception) {
        exceptionStack.push_back(std::make_exception_ptr(exception));
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


std::string ExceptionStack::what(const std::exception& exception)
{
    return what(get(exception));
}


std::string ExceptionStack::what(const ExceptionStack_t& exceptionStack)
{
    std::stringstream errorMessage;
    for (size_t i = 0; i < exceptionStack.size(); i++)
    {
        for (size_t j = 0; j < i; j++)
            errorMessage << '\t';
        try
        {
            std::rethrow_exception(exceptionStack[i]);
        }
        catch (const std::exception& exception)
        {
            errorMessage << exception.what() << std::endl;
        }
        catch (...)
        {
            errorMessage << "Unexpected" << std::endl;
        }
    }
    return errorMessage.str();
}