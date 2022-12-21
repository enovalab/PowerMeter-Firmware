#include "Measuring/ACPower.h"

#include <gtest/gtest.h>

constexpr float voltage = 230.0f;
constexpr float current = 1.0f;
constexpr float activePower = 100.0f;
Measuring::ACPower uut(voltage, current, activePower);

TEST(ACPowerTest, getValues)
{
    EXPECT_FLOAT_EQ(230.0f, uut.getVoltageRms());
    EXPECT_FLOAT_EQ(1.0f, uut.getCurrentRms());
    EXPECT_FLOAT_EQ(100.0f, uut.getActivePower());
    EXPECT_FLOAT_EQ(230.0f, uut.getApparentPower());
    EXPECT_FLOAT_EQ(207.12315f, uut.getReactivePower());
    EXPECT_FLOAT_EQ(0.43478259f, uut.getPowerFactor());
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}