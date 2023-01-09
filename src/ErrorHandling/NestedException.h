#ifndef ERRORHANDLING_NESTEDEXCEPTION_H
#define ERRORHANDLING_NESTEDEXCEPTION_H

#include <string>
#include <exception>

namespace ErrorHandling
{
    namespace NestedException
    {
        void rethrowMostNested();
        void throwWithNested();
        std::string what(uint8_t indentLevel = 1, char indentChar = ' ');
    }
}

#endif