#include "Measuring/Tracker.h"
#include "Measuring/Tracker.cpp"

#include "MockClock.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

using Measuring::Tracker;

constexpr const char* testFilePath = "TrackerTest.json";
MockClock mockClock;
Tracker uut(mockClock, testFilePath, 500, 5);


std::string getFileContent(std::ifstream& file)
{
    std::string content;
    content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return content;
}


TEST(TrackerTest, noTimeElapsed)
{
    uut << 1.0f;
    std::ifstream testFile(testFilePath);
    EXPECT_STREQ("", getFileContent(testFile).c_str());
}

TEST(TrackerTest, oneTimeElapsed)
{
    mockClock.setTime(101);
    uut << 3.0f;
    std::ifstream testFile(testFilePath);
    json actual = json::parse(testFile);

    EXPECT_TRUE(actual.is_array());
    EXPECT_EQ(1, actual.size());
    EXPECT_EQ(2.0f, actual.at(0));
}

TEST(TrackerTest, multipleTimesElapsed)
{
    mockClock.setTime(1000);
    uut << 2.0f;
    std::ifstream testFile(testFilePath);
    json actual = json::parse(testFile);  
    EXPECT_EQ(5, actual.size());
    for(size_t i = 0; i < actual.size() - 1; i++)
    {
        EXPECT_EQ(0, actual.at(i));
    }
}

TEST(TrackerTest, deleteTestFile)
{
    std::filesystem::remove(testFilePath);
}

int main()
{
    testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
