#include <gtest/gtest.h>
#include "system_coordinator.h"
#include "wdt_mock.h"

class SystemCoordinatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        wdt_mock_reset();
    }

    system_coordinator_t sys;
};

TEST_F(SystemCoordinatorTest, Init_NullSysReturnsFalse) {
    EXPECT_FALSE(system_coordinator_init(NULL));
}

TEST_F(SystemCoordinatorTest, Init_SuccessReturnsTrue) {
    EXPECT_TRUE(system_coordinator_init(&sys));
}

TEST_F(SystemCoordinatorTest, Init_WatchdogTaskIDsAreValidAndDistinct) {
    EXPECT_TRUE(system_coordinator_init(&sys));

    EXPECT_GE(sys.motion_wdt_id, 0);
    EXPECT_GE(sys.comms_wdt_id, 0);
    EXPECT_GE(sys.io_wdt_id, 0);

    EXPECT_NE(sys.motion_wdt_id, sys.comms_wdt_id);
    EXPECT_NE(sys.motion_wdt_id, sys.io_wdt_id);
    EXPECT_NE(sys.comms_wdt_id, sys.io_wdt_id);
}

TEST_F(SystemCoordinatorTest, Init_MotorManagerConfiguredCorrectly) {
    EXPECT_TRUE(system_coordinator_init(&sys));

    EXPECT_EQ(sys.motor_mgr.motor_count, MOTOR_COUNT);
    EXPECT_EQ(sys.motor_mgr.motors, sys.motors);
    EXPECT_EQ(sys.motor_mgr.protocol, &sys.protocol_adapter);

    for (int i = 0; i < MOTOR_COUNT; i++) {
        EXPECT_EQ(sys.motors[i].node_id, i + 1);
        EXPECT_FALSE(sys.motors[i].enabled);
    }
}

TEST_F(SystemCoordinatorTest, Init_FieldbusRouterConfiguredCorrectly) {
    EXPECT_TRUE(system_coordinator_init(&sys));

    EXPECT_EQ(sys.router.type, FIELDBUS_TYPE_CANFD);
    EXPECT_EQ(sys.router.can_dev, &sys.can_dev);
}

TEST_F(SystemCoordinatorTest, Init_HeartbeatConfiguredCorrectly) {
    EXPECT_TRUE(system_coordinator_init(&sys));

    EXPECT_EQ(sys.heartbeat.timeout_threshold_ms, (uint32_t)HOST_HEARTBEAT_TIMEOUT_MS);
    EXPECT_EQ(sys.heartbeat.heartbeat_counter, 0u);
}

TEST_F(SystemCoordinatorTest, Init_BrakeControllerConfiguredCorrectly) {
    EXPECT_TRUE(system_coordinator_init(&sys));

    EXPECT_EQ(sys.brake.gpio, &sys.brake_gpio);
    EXPECT_EQ(sys.brake.delay_ms, 100u);
    EXPECT_EQ(sys.brake.state, BRAKE_STATE_ENGAGED);
}

TEST_F(SystemCoordinatorTest, Init_SensorProcessorConfiguredCorrectly) {
    EXPECT_TRUE(system_coordinator_init(&sys));

    EXPECT_EQ(sys.sensor_proc.sensor, &sys.force_sensor);
    EXPECT_EQ(sys.sensor_proc.endstop, &sys.endstop_gpio);
    EXPECT_FLOAT_EQ(sys.sensor_proc.alpha, SENSOR_ALPHA_FILTER);
}

TEST_F(SystemCoordinatorTest, Init_HardwareInterfacesHaveVtables) {
    EXPECT_TRUE(system_coordinator_init(&sys));

    EXPECT_TRUE(sys.hw_wdt.init != NULL);
    EXPECT_TRUE(sys.hw_wdt.feed != NULL);
    EXPECT_TRUE(sys.can_dev.init != NULL);
    EXPECT_TRUE(sys.can_dev.send != NULL);
    EXPECT_TRUE(sys.brake_gpio.init != NULL);
    EXPECT_TRUE(sys.brake_gpio.write != NULL);
    EXPECT_TRUE(sys.protocol_adapter.init != NULL);
    EXPECT_TRUE(sys.protocol_adapter.send_controlword != NULL);
}
