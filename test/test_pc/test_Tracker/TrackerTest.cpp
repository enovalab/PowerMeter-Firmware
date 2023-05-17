#include "Data/Tracker.h"
#include "MockClock.h"

#include <gtest/gtest.h>

using namespace Data;

struct TrackerTest : public testing::Test
{
    void TearDown() override
    {
        std::filesystem::remove_all("TrackerTest");
    }

    MockClock clock;
    // Tracker uut = Tracker(clock, );
};

TEST_F(TrackerTest, )
{
    
}

int main()
{
    testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
