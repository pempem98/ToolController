#include <gtest/gtest.h>
#include "canopen_od.h"

TEST(CANopenODTest, InitStandardCiA402Registers) {
    canopen_od_t od;
    EXPECT_TRUE(canopen_od_init(&od));

    uint8_t len = 0;
    uint16_t controlword = 0;
    EXPECT_TRUE(canopen_od_read(&od, OD_IDX_CONTROLWORD, 0, &controlword, &len));
    EXPECT_EQ(len, sizeof(uint16_t));
}

TEST(CANopenODTest, ReadWriteTargetPosition) {
    canopen_od_t od;
    canopen_od_init(&od);

    int32_t set_pos = 1234567;
    EXPECT_TRUE(canopen_od_write(&od, OD_IDX_TARGET_POSITION, 0, &set_pos, sizeof(int32_t)));

    int32_t get_pos = 0;
    uint8_t len = 0;
    EXPECT_TRUE(canopen_od_read(&od, OD_IDX_TARGET_POSITION, 0, &get_pos, &len));
    EXPECT_EQ(get_pos, 1234567);
}

TEST(CANopenODTest, ReadOnlyRegisterWriteFails) {
    canopen_od_t od;
    canopen_od_init(&od);

    int32_t act_pos = 9999;
    EXPECT_FALSE(canopen_od_write(&od, OD_IDX_POSITION_ACTUAL_VALUE, 0, &act_pos, sizeof(int32_t)));
}
