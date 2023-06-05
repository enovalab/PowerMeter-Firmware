#include "Data/Tracker.h"
#include "MockClock.h"
#include "Data/AverageAccumulator.h"
#include "Diagnostics/ExceptionTrace.h"

#include <gtest/gtest.h>

using namespace Data;

struct TrackerTest : public testing::Test
{
    void SetUp() override
    {
        std::filesystem::create_directory("TrackerTest");
    }

    void TearDown() override
    {
        // std::filesystem::remove_all("TrackerTest");
    }

    MockClock mockClock;
    Tracker uut = Tracker(
        "Test Tracker",
        3600,
        60,
        mockClock,
        JsonURI("TrackerTest/data.json"),
        JsonURI("TrackerTest/lastSample.json"),
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


TEST_F(TrackerTest, track)
{
    for(size_t i = 0; i < 3600; i++)
    {
        uut.track(1.0f);
        mockClock.setTimestamp(mockClock.now() + 1);
    }
}

int main()
{
    testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
