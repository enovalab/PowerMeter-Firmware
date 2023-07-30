#include "ExceptionTrace.h"

#include <vector>
#include <sstream>


namespace
{
    std::vector<std::string> traces;
}


void ExceptionTrace::trace(const std::string& message) noexcept
{
    traces.push_back(message);
}


void ExceptionTrace::clear() noexcept
{
    traces.clear();
}


std::string ExceptionTrace::what(size_t indentLevel, char indentChar)
{
    std::stringstream what;

    for(size_t i = 0; i < traces.size(); i++)
        what << std::string(indentLevel * i, indentChar) << traces.at(traces.size() - i - 1) << '\n';
    
    try
    {
        std::exception_ptr currentException = std::current_exception();
        if(currentException)
            std::rethrow_exception(currentException);
    }
    catch(const std::exception& e)
    {
        what << std::string(indentLevel * traces.size(), indentChar) << e.what() << '\n';
    }
    catch(int e)
    {
        what << std::string(indentLevel * traces.size(), indentChar) << e << '\n';
    }
    catch(const char* e)
    {
        what << std::string(indentLevel * traces.size(), indentChar) << e << '\n';
    }
    catch(...)
    {
        what << std::string(indentLevel * traces.size(), indentChar) << "Unexpected Exception" << '\n';
    }
    
    traces.clear();
        
    return what.str();
}