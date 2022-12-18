#include "Measuring/StreamActivePower.h"

#include <gtest/gtest.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
 
using Measuring::StreamActivePower;

struct StreamActivePowerTest : public testing::Test
{
    StreamActivePower<float> uut;
};

TEST_F(StreamActivePowerTest, shoulInitializeToZero)
{
    EXPECT_FLOAT_EQ(0, uut.get());
    EXPECT_FLOAT_EQ(0, uut.getNumValues());
}

TEST_F(StreamActivePowerTest, addSineWithPhaseShift)
{
    const float apparentPower = 0.5;
    for(float radShift = -M_PI / 2; radShift < M_PI / 2; radShift += 0.01)
    {
        StreamActivePower<float> uut;
        for(float rad = 0.0f; rad < 2 * M_PI; rad += 0.01)
        {
            uut << sinf(rad) * sinf(rad + radShift);
        }
        float expectedActivePower = cosf(radShift) * apparentPower;
        EXPECT_NEAR(expectedActivePower, uut.get(), 0.01);
    }
}

TEST_F(StreamActivePowerTest, shouldResetToZero)
{
    uut << 12.3;
    uut << 12.5;
    uut.reset();
    EXPECT_FLOAT_EQ(0, uut.get());
    EXPECT_FLOAT_EQ(0, uut.getNumValues());
    EXPECT_FLOAT_EQ(0, uut.getNumValues());
}

int main()
{
    testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}