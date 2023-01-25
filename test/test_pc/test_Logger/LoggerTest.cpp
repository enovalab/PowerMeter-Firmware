#include "Diagnostics/Log.h"

#include <gtest/gtest.h>
#include <sstream>
#include <fstream>

using Diagnostics::Log;

TEST(LogTest, level)
{
    std::stringstream stream;
    Log uut(Level::Warning, &stream);
    uut[Level::Info] << "shouldn't log";
    uut[Level::Silent] << "shouldn't log";
    uut[Level::Error] << "error";
    uut[Level::Warning] << "warning";
    EXPECT_STREQ("errorwarning", stream.str().c_str());
}

TEST(LogTest, showLevel)
{
    std::stringstream stream;
    Log uut(Level::Verbose, &stream, true);
    uut[Level::Info] << "message";
    EXPECT_STREQ("[INFO] message", stream.str().c_str());
}

TEST(LogTest, notShowLevel)
{
    std::stringstream stream;
    Log uut(Level::Verbose, &stream);
    uut[Level::Info] << "message";
    EXPECT_STREQ("message", stream.str().c_str());
}


int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}