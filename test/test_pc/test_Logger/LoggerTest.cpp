#include "Diagnostics/Log.h"

#include <gtest/gtest.h>
#include <sstream>
#include <fstream>

using Diagnostics::Log;

class MultiStreamBuffer : public std::streambuf
{
public:
    void add(std::streambuf* streamBuffer)
    {
        m_streamBuffers.push_back(streamBuffer);
    }

    int overflow(int character) override
    {
        for(auto streamBuffer : m_streamBuffers)
        {
            int result = streamBuffer->sputc(character);
            if(result == EOF)
                return EOF;
        }
        return character;
    }

    int sync() override
    {
        for(auto streamBuffer : m_streamBuffers)
        {
            if(streamBuffer->pubsync() == -1)
                return -1;
        }
        return 0;
    }

private:
    std::vector<std::streambuf*> m_streamBuffers;
};

struct LogTest : public testing::Test
{
    void SetUp() override
    {
        multiStreamBuffer.add(std::cout.rdbuf());
        multiStreamBuffer.add(testStream.rdbuf());
    }

    std::stringstream testStream;
    MultiStreamBuffer multiStreamBuffer;
    std::ostream multiStream = std::ostream(&multiStreamBuffer);
    Log uut = Log(Level::Verbose, &multiStream);
};


TEST_F(LogTest, level)
{
    uut.setLevel(Level::Warning);
    uut[Level::Info] << "shouldn't log";
    uut[Level::Silent] << "shouldn't log";
    uut[Level::Error] << "error";
    uut[Level::Warning] << "warning";
    EXPECT_EQ("errorwarning", testStream.str());
}

TEST_F(LogTest, showLevel)
{
    uut.setShowLevel(true);
    uut[Level::Info] << "message";
    EXPECT_STREQ("[INFO] message", testStream.str().c_str());
}

TEST_F(LogTest, notShowLevel)
{
    uut.setShowLevel(false);
    uut[Level::Info] << "message";
    EXPECT_STREQ("message", testStream.str().c_str());
}


int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}