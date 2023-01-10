#include "ErrorHandling/ExceptionTrace.h"

#include <deque>
#include <sstream>

using namespace ErrorHandling;


static std::deque<std::string> traces;


void ExceptionTrace::trace(const std::string& message)
{
    traces.push_front(message);
}


std::string ExceptionTrace::what(size_t indentLevel, char indentChar)
{
    std::stringstream what;

    for(size_t i = 0; i < traces.size(); i++)
        what << std::string(indentLevel * i, indentChar) << traces[i] << '\n';
    
    try
    {
        throw;
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