#include "ErrorHandling/NestedException.h"

#include <functional>
#include <exception>
#include <sstream>

using namespace ErrorHandling;


void rethrowIfNestedRecursive(const std::exception& exception)
{
    try
    {
        std::rethrow_if_nested(exception);
    }
    catch(const std::exception& nestedException)
    {
        rethrowIfNestedRecursive(nestedException);
    }
}

void NestedException::rethrowMostNested()
{
    try
    {
        throw;
    }
    catch(const std::exception& e)
    {
        rethrowIfNestedRecursive(e);
    }
}


// std::string NestedException::what(uint8_t indentLevel, char indentChar)
// {
//     std::stringstream what;
//     try
//     {
//         throw;
//     }
//     catch(const std::exception& exception)
//     {
//         uint8_t nestingLevel = 0;
//         std::function<void(const std::exception&)> appendToWhat = [&](const std::exception& exception) {
//             what << std::string(static_cast<size_t>(nestingLevel), indentChar) << exception.what() << '\n';
//             nestingLevel += indentLevel;
//             try
//             {
//                 std::rethrow_if_nested(exception);
//             }
//             catch (const std::exception& nestedException)
//             {
//                 appendToWhat(nestedException);
//             }
//         };
//         appendToWhat(exception);
//     }

//     return what.str();
// }