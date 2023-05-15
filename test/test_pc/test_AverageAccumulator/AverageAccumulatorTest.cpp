#include "Data/AverageAccumulator.h"

#include <gtest/gtest.h>

using Data::AverageAccumulator;
using Data::JsonURI;

struct AverageAccumulatorTest : public testing::Test
{
    AverageAccumulator uut = AverageAccumulator(JsonURI("AverageAccumulatorTest.json"));
};

TEST_F(AverageAccumulatorTest, shouldInitializeToZero)
{
    EXPECT_EQ(0, uut.get());
}