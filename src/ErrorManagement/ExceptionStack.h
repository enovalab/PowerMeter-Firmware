#ifndef ERRORMANAGEMENT_EXCEPTIONSTACK_H
#define ERRORMANAGEMENT_EXCEPTIONSTACK_H

#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

namespace ErrorManagement
{
    namespace ExceptionStack
    {
        using ExceptionStack_t = std::vector<std::exception_ptr>;

        std::vector<std::exception_ptr> get(const std::exception& exception);
        std::string what(const std::exception& exception);
        std::string what(const std::vector<std::exception_ptr>& exceptionStack);
    }
}

#endif