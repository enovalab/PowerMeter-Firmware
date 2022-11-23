#include "Measuring/StreamActivePower.h"

#include <gtest/gtest.h>
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846
#endif


TEST(StreamActivePowerTest, shoulInitializeToZero)
{
    StreamActivePower<float> uut;
    EXPECT_FLOAT_EQ(0, uut.get());
    EXPECT_FLOAT_EQ(0, uut.getNumValues());
}

TEST(StreamActivePowerTest, addSineWithPhaseShift)
{
    const float apparentPower = 0.5;
    for(float radShift = -PI / 2; radShift < PI / 2; radShift += 0.01)
    {
        StreamActivePower<float> uut;
        for(float rad = 0.0f; rad < 2 * PI; rad += 0.01)
        {
            uut << sinf(rad) * sinf(rad + radShift);
        }
        float expectedActivePower = cosf(radShift) * apparentPower;
        EXPECT_NEAR(expectedActivePower, uut.get(), 0.01);
    }
    
}

TEST(StreamActivePowerTest, shouldResetToZero)
{
    StreamActivePower<float> uut;
    uut << 12.3;
    uut << 12.5;
    uut.reset();
    EXPECT_FLOAT_EQ(0, uut.get());
    EXPECT_FLOAT_EQ(0, uut.getNumValues());
    EXPECT_FLOAT_EQ(0, uut.getNumValues());
}