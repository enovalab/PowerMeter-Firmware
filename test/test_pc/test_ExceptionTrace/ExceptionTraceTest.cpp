#include "Diagnostics/ExceptionTrace.h"

#include <exception>
#include <iostream>
#include <gtest/gtest.h>

using namespace Diagnostics;

void c()
{
    try
    {
        throw std::runtime_error("foo");
    }
    catch(...)
    {
        ExceptionTrace::trace("c");
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
        ExceptionTrace::trace("b");
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
        ExceptionTrace::trace("a");
        throw;
    }
}

void b1()
{
    try
    {
        c();
    }
    catch(const std::runtime_error& e)
    {
        EXPECT_TRUE(true) << "Caught runtime_error" << std::endl;
    }
    
}

void a1()
{
    try
    {
        b1();
    }
    catch(...)
    {
        ExceptionTrace::trace("a");
        throw;
    }
    
}


TEST(ExceptionTraceTest, normalTrace)
{
    try
    {
        a();
    }
    catch(...)
    {
        EXPECT_EQ("a\nxb\nxxc\nxxxfoo\n", ExceptionTrace::what(1, 'x'));
    }
}


TEST(ExceptionTraceTest, catchInbetween)
{
    try
    {
        a1();
    }
    catch(...)
    {
        FAIL() << "Should have been caught yet" << std::endl;
    }
}


TEST(ExceptionTraceTest, clearOnWhat)
{
    ExceptionTrace::trace("xyz");
    ExceptionTrace::what();
    EXPECT_EQ("", ExceptionTrace::what());
}


TEST(ExceptionTraceTest, clear)
{
    ExceptionTrace::trace("xyz");
    ExceptionTrace::clear();
    EXPECT_EQ("", ExceptionTrace::what());
}


int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}