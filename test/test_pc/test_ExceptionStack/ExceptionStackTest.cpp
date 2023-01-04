#include <iostream>
#include <exception>
#include <json.hpp>

template<typename E>
void rethrowIfNestedRecursive(const E& e)
{
    try
    {
        std::rethrow_if_nested(e);
    }
    catch(const std::exception& ex)
    {
        rethrowIfNestedRecursive(ex);
    }
}

void rethrowMostNested()
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

void b()
{
    try
    {
        json data;
        data.at("does not exist");
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
    catch(const std::exception& nestedException)
    {
        std::cout << nestedException.what() << std::endl;
        try
        {
            rethrowIfNestedRecursive(nestedException);
        }
        catch(const json::out_of_range& e)
        {
            std::cout << "Caught json exception" << std::endl;
            std::cout << e.what() << std::endl;
        }
        catch(const std::exception& e)
        {
            std::cout << "Caught any" << std::endl;
            std::cout << e.what() << std::endl;
        }
    }
}