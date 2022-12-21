#include "Error/ExceptionStack.h"

#include <gtest/gtest.h>

using namespace Error;

void c()
{
    try
    {
        throw std::runtime_error("d");
    }
    catch(...)
    {
        std::throw_with_nested(std::runtime_error("c"));
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
        std::throw_with_nested(std::runtime_error("b"));
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
        std::throw_with_nested(std::runtime_error("a"));
    }
}

TEST(ExceptionStackTest, getExceptionStack)
{
    try
    {
        a();
    }
    catch(const std::exception& e)
    {
        ExceptionStack_t exceptionStack = ExceptionStack::get(e);
        EXPECT_EQ(4, exceptionStack.size());
        EXPECT_EQ("a\n b\n  c\n   d\n", ExceptionStack::what(exceptionStack, 1, ' '));
    }
}

TEST(ExceptionStackTest, getWhat)
{
    try
    {
        a();
    }
    catch(const std::exception& e)
    {
        EXPECT_EQ("a\n b\n  c\n   d\n", ExceptionStack::what(e, 1, ' '));
    }
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}