#include <gtest/gtest.h>
#include "brake_controller.h"
#include "gpio_mock.h"

class BrakeControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        gpio_mock_create(&gpio);
        g_mock_gpio_state = GPIO_STATE_LOW;
    }

    gpio_interface_t gpio;
    brake_controller_t brake;
};

TEST_F(BrakeControllerTest, Init_Success) {
    EXPECT_TRUE(brake_controller_init(&brake, &gpio, 100));
    EXPECT_EQ(brake.gpio, &gpio);
    EXPECT_EQ(brake.delay_ms, 100u);
    EXPECT_EQ(brake.state, BRAKE_STATE_ENGAGED);
    EXPECT_EQ(g_mock_gpio_state, GPIO_STATE_LOW);
}

TEST_F(BrakeControllerTest, Init_NullBrake) {
    EXPECT_FALSE(brake_controller_init(NULL, &gpio, 100));
}

TEST_F(BrakeControllerTest, Init_NullGpio) {
    EXPECT_FALSE(brake_controller_init(&brake, NULL, 100));
}

TEST_F(BrakeControllerTest, Release_Success) {
    brake_controller_init(&brake, &gpio, 100);

    EXPECT_TRUE(brake_release(&brake));
    EXPECT_EQ(brake.state, BRAKE_STATE_RELEASED);
    EXPECT_EQ(g_mock_gpio_state, GPIO_STATE_HIGH);
}

TEST_F(BrakeControllerTest, Release_NullBrake) {
    EXPECT_FALSE(brake_release(NULL));
}

TEST_F(BrakeControllerTest, Release_NullGpioInStruct) {
    brake_controller_init(&brake, &gpio, 100);
    brake.gpio = NULL;
    EXPECT_FALSE(brake_release(&brake));
}

TEST_F(BrakeControllerTest, Engage_Success) {
    brake_controller_init(&brake, &gpio, 100);
    brake_release(&brake);
    EXPECT_EQ(g_mock_gpio_state, GPIO_STATE_HIGH);

    EXPECT_TRUE(brake_engage(&brake));
    EXPECT_EQ(brake.state, BRAKE_STATE_ENGAGED);
    EXPECT_EQ(g_mock_gpio_state, GPIO_STATE_LOW);
}

TEST_F(BrakeControllerTest, Engage_NullBrake) {
    EXPECT_FALSE(brake_engage(NULL));
}

TEST_F(BrakeControllerTest, GetState_NullBrake) {
    EXPECT_EQ(brake_get_state(NULL), BRAKE_STATE_ENGAGED);
}

TEST_F(BrakeControllerTest, GetState_AfterInitAndRelease) {
    brake_controller_init(&brake, &gpio, 100);
    EXPECT_EQ(brake_get_state(&brake), BRAKE_STATE_ENGAGED);

    brake_release(&brake);
    EXPECT_EQ(brake_get_state(&brake), BRAKE_STATE_RELEASED);

    brake_engage(&brake);
    EXPECT_EQ(brake_get_state(&brake), BRAKE_STATE_ENGAGED);
}
