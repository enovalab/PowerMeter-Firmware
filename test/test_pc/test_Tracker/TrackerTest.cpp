#include "Measuring/Tracker.h"
#include "Measuring/Tracker.cpp"

#include "MockClock.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

using Measuring::Tracker;
using Measuring::TrackingSpan;
using System::JsonResource;

constexpr const char* testFilePath = "TrackerTest.json";
MockClock mockClock;

Tracker uut(mockClock, {
    TrackingSpan(
        JsonResource("TrackerTest.json#/last60min"),
        JsonResource("TrackerTest.json#/lastSamples/last60min"), 
        3600, 
        60
    ),
    TrackingSpan(
        JsonResource("TrackerTest.json#/last24h"), 
        JsonResource("TrackerTest.json#/lastSamples/last24h"), 
        3600 * 24, 
        24,
        JsonResource("TrackerTest.json#/last60min")
    ),
    TrackingSpan(
        JsonResource("TrackerTest.json#/last7d"), 
        JsonResource("TrackerTest.json#/lastSamples/last7d"), 
        3600 * 24 * 7, 
        7,
        JsonResource("TrackerTest.json#/last24h")
    ),
    TrackingSpan(
        JsonResource("TrackerTest.json#/last30d"), 
        JsonResource("TrackerTest.json#/lastSamples/last30d"), 
        3600 * 24 * 30, 
        30,
        JsonResource("TrackerTest.json#/last24h")
    ),
    TrackingSpan(
        JsonResource("TrackerTest.json#/last12m"), 
        JsonResource("TrackerTest.json#/lastSamples/last12m"), 
        3600 * 24 * 30 * 12, 
        12,
        JsonResource("TrackerTest.json#/last24h")
    ),
    TrackingSpan(
        JsonResource("TrackerTest.json#/last10a"), 
        JsonResource("TrackerTest.json#/lastSamples/las10a"), 
        3600 * 24 * 30 * 12 * 10, 
        10,
        JsonResource("TrackerTest.json#/last12m")
    ),
});


TEST(TrackerTest, noTimeElapsed)
{
    uut.track(1.0f);
    EXPECT_TRUE(std::ifstream("TrackerTest.json").peek() < 0);
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
