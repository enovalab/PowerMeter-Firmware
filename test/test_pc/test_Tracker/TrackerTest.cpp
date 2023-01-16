#include "Data/Tracker.h"
#include "MockClock.h"
#include "ErrorHandling/ExceptionTrace.h"

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>

using namespace Data;
using namespace std::chrono_literals;

struct TrackerTest : public testing::Test
{
    void TearDown() override
    {
        std::filesystem::remove("TrackerTest.json");
    }

    std::vector<TrackingSpan> trackingSpans = {
        TrackingSpan(
            JsonURI("TrackerTest.json#/last60min"),
            JsonURI("TrackerTest.json#/lastSamples/last60min"), 
            3600, 
            60
        ),
        TrackingSpan(
            JsonURI("TrackerTest.json#/last24h"), 
            JsonURI("TrackerTest.json#/lastSamples/last24h"), 
            3600 * 24, 
            24,
            JsonURI("TrackerTest.json#/last60min")
        ),
        TrackingSpan(
            JsonURI("TrackerTest.json#/last7d"), 
            JsonURI("TrackerTest.json#/lastSamples/last7d"), 
            3600 * 24 * 7, 
            7,
            JsonURI("TrackerTest.json#/last24h")
        )
    };
    MockClock clock;
    Tracker uut = Tracker(clock, trackingSpans);
};

TEST_F(TrackerTest, lastSampleShouldInitializeToZero)
{
    try
    {
        for(const auto& trackingSpan : trackingSpans)
        {
            EXPECT_EQ(0, trackingSpan.getLastSampleTimestamp());
        }
    }
    catch(...)
    {
        FAIL() << ErrorHandling::ExceptionTrace::what() << std::endl;
    }
}

TEST_F(TrackerTest, shouldFillWithZero)
{
    try
    {
        clock.setTimestamp(3601);
        uut.track(1.0f);
        json last60min = JsonURI("TrackerTest.json#/last60min").deserialize();
        for(size_t i = 0; i < last60min.size() - 1; i++)
        {
            EXPECT_EQ(0, last60min.at(i));
        }
        EXPECT_EQ(1.0f, last60min.back());

        json last24h = JsonURI("TrackerTest.json#/last24h").deserialize();
        EXPECT_EQ(1.0f / 60, last24h.front());
    }
    catch(...)
    {
        FAIL() << ErrorHandling::ExceptionTrace::what() << std::endl;
    }
}

TEST_F(TrackerTest, arraySizeNotGreaterThanSpecified)
{
    try
    {
        while(clock.now() < 3600 * 24 * 7 + 1)
        {
            uut.track(1.0f);
            clock.setTimestamp(clock.now() + 10);
        }

        json last60min = JsonURI("TrackerTest.json#/last60min").deserialize();
        EXPECT_TRUE(last60min.is_array());
        EXPECT_EQ(60, last60min.size());

        json last24h = JsonURI("TrackerTest.json#/last24h").deserialize();
        EXPECT_TRUE(last24h.is_array());
        EXPECT_EQ(24, last24h.size());

        json last7d = JsonURI("TrackerTest.json#/last7d").deserialize();
        EXPECT_TRUE(last7d.is_array());
        EXPECT_EQ(7, last7d.size());
    }
    catch(...)
    {
        FAIL() << ErrorHandling::ExceptionTrace::what() << std::endl;
    }
}

int main()
{
    testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
