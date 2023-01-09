#include "ErrorHandling/ExceptionTrace.h"

#include <exception>
#include <iostream>
#include <gtest/gtest.h>

void c()
{
    try
    {
        throw "foo";
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


TEST()
{
    
}


int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}