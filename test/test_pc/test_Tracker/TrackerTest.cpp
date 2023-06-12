#include "Data/Tracker.h"
#include "MockClock.h"
#include "Data/AverageAccumulator.h"
#include "Diagnostics/ExceptionTrace.h"
#include "Diagnostics/Log.h"

#include <gtest/gtest.h>
#include <ctime>

using namespace Data;

constexpr time_t duration_s = 3600;
constexpr size_t sampleCount = 60;


struct TrackerTest : public testing::Test
{
    void SetUp() override
    {
        std::filesystem::create_directory("TrackerTest");
    }

    void TearDown() override
    {
        std::filesystem::remove_all("TrackerTest");
    }

    MockClock mockClock = MockClock(time(nullptr));
    Tracker uut = Tracker(
        "Test Tracker",
        duration_s,
        sampleCount,
        mockClock,
        JsonURI("TrackerTest/data.json"),
        JsonURI("TrackerTest/timestamps.json#/lastInput"),
        JsonURI("TrackerTest/timestamps.json#/lastSample"),
        AverageAccumulator(JsonURI("TrackerTest/accumulator.json"))
    );
};

// TEST_F(TrackerTest, checkGetDataOfEmptyTracker)
// {
//     try
//     {
//         json expectedData;
//         expectedData["data"] = json::array_t();
//         expectedData["duration_s"] = 3600;
//         expectedData["sampleCount"] = 60;
//         expectedData["title"] = "Test Tracker";
//         EXPECT_EQ(expectedData, uut.getData());
//     }
//     catch(...)
//     {
//         FAIL() << Diagnostics::ExceptionTrace::what() << std::endl;
//     }
// }


// TEST_F(TrackerTest, track)
// {
//     for(size_t i = 0; i < 3601; i++)
//     {
//         uut.track(1.0f);
//         mockClock.tick();
//     }
// }


TEST_F(TrackerTest, shouldFillWhilePowerDown)
{
    for(size_t i = 0; i < 1000; i++)
    {
        uut.track(1.0f);
        mockClock.tick();
    }
    mockClock.tick(1000);
    for(size_t i = 0; i < 1620; i++)
    {
        uut.track(1.0f);
        mockClock.tick();
    }
    json data = uut.getData().at("data");
    EXPECT_EQ(data.size(), sampleCount);
}


int main()
{
    testing::InitGoogleTest();
    int a = RUN_ALL_TESTS();
	return a;
}