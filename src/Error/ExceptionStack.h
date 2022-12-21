#ifndef ERRORMANAGEMENT_EXCEPTIONSTACK_H
#define ERRORMANAGEMENT_EXCEPTIONSTACK_H

#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

namespace Error
{
    using ExceptionStack_t = std::vector<std::exception_ptr>;
    
    namespace ExceptionStack
    {
        std::vector<std::exception_ptr> get(const std::exception& exception);
        std::string what(const std::exception& exception, uint8_t indent = 0, char indentChar = ' ');
        std::string what(const std::vector<std::exception_ptr>& exceptionStack, uint8_t indent = 0, char indentChar = ' ');
    }
}

#endif