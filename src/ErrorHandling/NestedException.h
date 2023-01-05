#ifndef ERRORHANDLING_NESTEDEXCEPTION_H
#define ERRORHANDLING_NESTEDEXCEPTION_H

#include <string>
#include <exception>

namespace ErrorHandling
{
    namespace NestedException
    {
        void rethrowMostNested();
        std::string what(uint8_t indentLevel = 0, char indentChar = ' ');
    }
}

#endif