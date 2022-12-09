#include "Measuring/Tracker.h"
#include "Measuring/Tracker.cpp"
#include "System/JsonResource.h"
#include "System/JsonResource.cpp"
#include "Logging/Log.h"
#include "Logging/Log.cpp"

#include "MockClock.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>

using Measuring::Tracker;
using Measuring::TrackingSpan;
using System::JsonResource;

const std::vector<TrackingSpan> config = {
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
    )
};

TEST(TrackerTest, arraySizeNotGreaterThanSpecified)
{
    MockClock clock;
    Tracker uut(clock, config);

    // "Zehn Jahre" tracken
    while(clock.now() < 3600 * 24 * 30 * 12 * 10)
    {
        uut.track(1.0f);
        clock.setTimestamp(clock.now() + 1);
    }

    json last60min = JsonResource("TrackerTest.json#/last60min").deserialize();
    EXPECT_TRUE(last60min.is_array());
    EXPECT_EQ(60, last60min.size());

    json last24h = JsonResource("TrackerTest.json#/last24h").deserialize();
    EXPECT_TRUE(last24h.is_array());
    EXPECT_EQ(24, last24h.size());

    json last7d = JsonResource("TrackerTest.json#/last7d").deserialize();
    EXPECT_TRUE(last7d.is_array());
    EXPECT_EQ(7, last7d.size());

    json last30d = JsonResource("TrackerTest.json#/last30d").deserialize();
    EXPECT_TRUE(last30d.is_array());
    EXPECT_EQ(30, last30d.size());

    json last12m = JsonResource("TrackerTest.json#/last12m").deserialize();
    EXPECT_TRUE(last12m.is_array());
    EXPECT_EQ(12, last12m.size());

    json last10a = JsonResource("TrackerTest.json#/last10a").deserialize();
    EXPECT_TRUE(last10a.is_array());
    EXPECT_EQ(10, last10a.size());

    // std::filesystem::remove("TrackerTest.json");
}


TEST(TrackerTest, shouldFillWithZero)
{
    
}


int main()
{
    testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
