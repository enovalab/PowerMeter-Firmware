#include "ErrorHandling/NestedException.h"

#include <iostream>
#include <json.hpp>

void b()
{
    try
    {
        json data = {1, 12, 3};
        data.at(21);
        // throw 20390;
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error("a() failed"));
    }
}

void a()
{
    try
    {
        b();
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error("b() failed"));
    }
    
}

int main()
{
    try
    {
        a();
    }
    catch(...)
    {
        // std::cout << ErrorHandling::NestedException::what(1) << std::endl;
        try
        {
            ErrorHandling::NestedException::rethrowMostNested();
        }
        catch(const int& e)
        {
            std::cout << "Caught int exception" << std::endl;
            std::cout << e << std::endl;
        }
        catch(const json::out_of_range& e)
        {
            std::cout << "Caught json::exception" << std::endl;
            std::cout << e.what() << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cout << "Caught base exception" << std::endl;
            std::cout << e.what() << std::endl;
        }
        catch(...)
        {
            std::cout << "Caught any" << std::endl;
        }
    }
}