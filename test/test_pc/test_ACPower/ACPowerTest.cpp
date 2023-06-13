#include "Measuring/ACPower.h"

#include <gtest/gtest.h>

constexpr float voltage_Vrms = 230.0f;
constexpr float current_Arms = 1.0f;
constexpr float activePower_W = 100.0f;
Measuring::ACPower uut(voltage_Vrms, current_Arms, activePower_W);

TEST(ACPowerTest, getValues)
{
    EXPECT_FLOAT_EQ(230.0f, uut.getVoltage_Vrms());
    EXPECT_FLOAT_EQ(1.0f, uut.getCurrent_Arms());
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