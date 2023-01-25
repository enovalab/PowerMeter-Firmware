#ifndef DIAGNOSTICS_EXCEPTIONTRACE_H
#define DIAGNOSTICS_EXCEPTIONTRACE_H

#include <string>

namespace Diagnostics
{
    namespace ExceptionTrace
    {
        void trace(const std::string& message);
        void clear();
        std::string what(size_t indentLevel = 1, char indentChar = ' ');
    }
}

#endif