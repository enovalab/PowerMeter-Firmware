#include "Measuring/StreamRMS.h"

#include <gtest/gtest.h>
#include <cmath>

#ifndef PI
#define PI 3.14159265358979323846
#endif

TEST(StreamRMSTest, shoulInitializeToZero)
{
    StreamRMS<float> uut;
    EXPECT_FLOAT_EQ(0, uut.get());
    EXPECT_FLOAT_EQ(0, uut.getNumValues());
}

TEST(StreamRMSTest, addValues)
{
    StreamRMS<float> uut;
    for(float rad = 0.0f; rad < 2 * PI; rad += 0.01)
    {
        uut << sinf(rad);
    }
    EXPECT_NEAR(0.7071, uut.get(), 0.01);
}

TEST(StreamRMSTest, shouldResetToZero)
{
    StreamRMS<float> uut;
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