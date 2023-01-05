#include "ErrorHandling/NestedException.h"

#include <exception>
#include <iostream>
#include <json.hpp>


void b()
{
    try
    {
        throw -2211;
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
        std::cout << "EXCEPTION" << std::endl;
        try
        {
            ErrorHandling::NestedException::rethrowMostNested();
        }
        catch(const int& e)
        {
            std::cout << "Caught int" << std::endl;
            std::cout << e << std::endl;
        }
        catch(const json::exception& e)
        {
            std::cout << "Caught json::exception" << std::endl;
            std::cout << e.what() << std::endl;
        }
        catch(const std::runtime_error& e)
        {
            std::cout << "Caught std::runtime_error" << std::endl;
            std::cout << e.what() << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cout << "Caught std::exception" << std::endl;
            std::cout << e.what() << std::endl;
        }
        catch(...)
        {
            std::cout << "Caught any" << std::endl;
        }
    }
}