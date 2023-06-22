#include "Measuring/ACPower.h"

#include <gtest/gtest.h>

constexpr float voltageRMS_V = 230.0f;
constexpr float currentRMS_A = 1.0f;
constexpr float activePower_W = 100.0f;
Measuring::ACPower uut(voltageRMS_V, currentRMS_A, activePower_W);

TEST(ACPowerTest, getValues)
{
    EXPECT_FLOAT_EQ(230.0f, uut.getVoltageRMS_V());
    EXPECT_FLOAT_EQ(1.0f, uut.getCurrentRMS_A());
    EXPECT_FLOAT_EQ(100.0f, uut.getActivePower_W());
    EXPECT_FLOAT_EQ(230.0f, uut.getApparentPower_VA());
    EXPECT_FLOAT_EQ(207.12315f, uut.getReactivePower_var());
    EXPECT_FLOAT_EQ(0.43478259f, uut.getPowerFactor());
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}