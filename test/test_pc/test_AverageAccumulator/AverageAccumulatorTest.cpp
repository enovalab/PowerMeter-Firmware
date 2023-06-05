#include "Data/AverageAccumulator.h"

#include <gtest/gtest.h>
#include <math.h>
#include <vector>

#define PI 3.14159265

using Data::AverageAccumulator;
using Data::JsonURI;


struct AverageAccumulatorTest : public testing::Test
{
    void TearDown() override
    {
        std::filesystem::remove("AverageAccumulatorTest.json");
    }

    AverageAccumulator uut = AverageAccumulator(JsonURI("AverageAccumulatorTest.json"));
};


TEST_F(AverageAccumulatorTest, shouldInitializeToZero)
{
    EXPECT_EQ(0, uut.getCount());
    EXPECT_EQ(0.0f, uut.getAverage());
}


TEST_F(AverageAccumulatorTest, sineShouldHaveAverageOfZero)
{
    for(int degrees = 0; degrees < 360; degrees++)
    {
        float value = sinf(degrees * (PI / 180.0f));
        uut.add(value);
    }

    EXPECT_EQ(360, uut.getCount());
    EXPECT_NEAR(0.0f, uut.getAverage(), 0.00001f);
}


TEST_F(AverageAccumulatorTest, shouldResetToZero)
{
    uut.add(1233.4f);

    uut.reset();
    EXPECT_EQ(0, uut.getCount());
    EXPECT_EQ(0.0f, uut.getAverage());
}


int main()
{
    testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}