#include "ErrorHandling/NestedException.h"

#include <exception>
#include <iostream>
#include <type_traits>

struct MyError : public std::exception
{
    virtual const char* what() const noexcept override
    {
        return "MyError thrown: bla";
    }
};


void c()
{
    try
    {
        throw std::runtime_error("sjldfsjlfj");
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error("c() failed"));
    }
}


void b()
{
    try
    {
        c();
    }
    catch(...)
    {
        try
        {
            ErrorHandling::NestedException::rethrowMostNested();
        }
        catch(const MyError& e)
        {
            std::cout << "Handled MyError" << std::endl;
        }
        std::throw_with_nested(std::runtime_error("b() failed"));
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
        std::throw_with_nested(std::runtime_error("a() failed"));
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
        std::cout << ErrorHandling::NestedException::what() << std::endl;
        try
        {
            ErrorHandling::NestedException::rethrowMostNested();
        }
        catch(const int& e)
        {
            std::cout << "Caught int" << std::endl;
            std::cout << e << std::endl;
        }
        catch(const MyError& e)
        {
            std::cout << "Caught MyError" << std::endl;
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
    std::cout << std::boolalpha;
    std::cout << std::is_base_of<std::exception, std::runtime_error>::value << std::endl;
}