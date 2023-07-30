#ifndef EXCEPTIONTRACE_H
#define EXCEPTIONTRACE_H

#include <string>


namespace ExceptionTrace
{
    void trace(const std::string& message) noexcept;
    void clear() noexcept;
    std::string what(size_t indentLevel = 1, char indentChar = ' ');
}

#endif