#include "ErrorHandling/ExceptionTrace.h"

#include <exception>
#include <iostream>


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
        throw "bla blah";
    }
    catch(...)
    {
        ErrorHandling::ExceptionTrace::trace("c() failed");
        throw;
    }
}


void b()
{
    try
    {
        c();
    }
    catch(const MyError& e)
    {
        std::cout << "Caught MyError: " << e.what() << std::endl;
    }
    catch(...)
    {
        ErrorHandling::ExceptionTrace::trace("b() failed");
        throw;
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
        ErrorHandling::ExceptionTrace::trace("a() failed");
        throw;
    }
    
}


int main()
{
    try
    {
        a();
    }
    catch(const MyError& e)
    {
        std::cout << "Caught MyError in main" << std::endl;
        std::cout << e.what() << std::endl;
    }
    catch(...)
    {
        std::cout << "Caught any" << std::endl;
        std::cout << ErrorHandling::ExceptionTrace::what() << std::endl;
    }


    try
    {
        a();
    }
    catch(const MyError& e)
    {
        std::cout << "Caught MyError in main" << std::endl;
        std::cout << e.what() << std::endl;
    }
    catch(...)
    {
        std::cout << "Caught any" << std::endl;
        std::cout << ErrorHandling::ExceptionTrace::what() << std::endl;
    }
}