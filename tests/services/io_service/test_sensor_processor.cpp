#include <gtest/gtest.h>
#include "sensor_processor.h"
#include "sensor_mock.h"
#include "gpio_mock.h"

class SensorProcessorTest : public ::testing::Test {
protected:
    void SetUp() override {
        sensor_mock_create(&sensor);
        gpio_mock_create(&endstop);
        sensor_mock_reset();
        g_mock_gpio_state = GPIO_STATE_LOW;
    }

    sensor_interface_t sensor;
    gpio_interface_t endstop;
    sensor_processor_t sp;
};

TEST_F(SensorProcessorTest, Init_Success) {
    EXPECT_TRUE(sensor_processor_init(&sp, &sensor, &endstop, 0.2f));
    EXPECT_EQ(sp.sensor, &sensor);
    EXPECT_EQ(sp.endstop, &endstop);
    EXPECT_FLOAT_EQ(sp.alpha, 0.2f);
    EXPECT_FLOAT_EQ(sp.filtered_force, 0.0f);
    EXPECT_FALSE(sp.endstop_triggered);
}

TEST_F(SensorProcessorTest, Init_NullSp) {
    EXPECT_FALSE(sensor_processor_init(NULL, &sensor, &endstop, 0.2f));
}

TEST_F(SensorProcessorTest, Update_ForceFilter) {
    sensor_processor_init(&sp, &sensor, &endstop, 0.2f);

    g_mock_sensor_force_val = 10.0f;
    EXPECT_TRUE(sensor_processor_update(&sp));
    EXPECT_FLOAT_EQ(sensor_processor_get_force(&sp), 2.0f);

    EXPECT_TRUE(sensor_processor_update(&sp));
    EXPECT_FLOAT_EQ(sensor_processor_get_force(&sp), 3.6f);
}

TEST_F(SensorProcessorTest, Update_ForceReadFailure) {
    sensor_processor_init(&sp, &sensor, &endstop, 0.2f);
    sp.filtered_force = 5.0f;

    g_mock_sensor_read_force_ret = false;
    EXPECT_TRUE(sensor_processor_update(&sp));
    EXPECT_FLOAT_EQ(sensor_processor_get_force(&sp), 5.0f);
}

TEST_F(SensorProcessorTest, Update_EndstopTriggered) {
    sensor_processor_init(&sp, &sensor, &endstop, 0.2f);

    g_mock_gpio_state = GPIO_STATE_HIGH;
    EXPECT_TRUE(sensor_processor_update(&sp));
    EXPECT_TRUE(sensor_processor_is_endstop_hit(&sp));

    g_mock_gpio_state = GPIO_STATE_LOW;
    EXPECT_TRUE(sensor_processor_update(&sp));
    EXPECT_FALSE(sensor_processor_is_endstop_hit(&sp));
}

TEST_F(SensorProcessorTest, Update_NullSp) {
    EXPECT_FALSE(sensor_processor_update(NULL));
}

TEST_F(SensorProcessorTest, Update_NullSensorNullEndstop) {
    sensor_processor_init(&sp, NULL, NULL, 0.2f);
    EXPECT_TRUE(sensor_processor_update(&sp));
    EXPECT_FLOAT_EQ(sensor_processor_get_force(&sp), 0.0f);
    EXPECT_FALSE(sensor_processor_is_endstop_hit(&sp));
}

TEST_F(SensorProcessorTest, GetForce_NullSp) {
    EXPECT_FLOAT_EQ(sensor_processor_get_force(NULL), 0.0f);
}

TEST_F(SensorProcessorTest, IsEndstopHit_NullSp) {
    EXPECT_FALSE(sensor_processor_is_endstop_hit(NULL));
}
