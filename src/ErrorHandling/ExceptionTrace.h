#ifndef ERRORHANDLING_EXCEPTIONTRACE_H
#define ERRORHANDLING_EXCEPTIONTRACE_H

#include <string>

namespace ErrorHandling
{
    namespace ExceptionTrace
    {
        void trace(const std::string& message);
        std::string what(size_t indentLevel = 1, char indentChar = ' ');
    }
}

#endif