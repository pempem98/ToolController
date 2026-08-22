#include <gtest/gtest.h>
#include "motor_manager.h"

extern "C" {
    void canfd_adapter_create(motor_protocol_adapter_t *adapter);
}

class MotorManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        canfd_adapter_create(&protocol);
    }

    motor_instance_t motors[4];
    motor_protocol_adapter_t protocol;
    motor_manager_t mgr;
};

TEST_F(MotorManagerTest, Initialization) {
    EXPECT_TRUE(motor_manager_init(&mgr, motors, 4, &protocol));
    EXPECT_EQ(mgr.motor_count, 4);

    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(mgr.motors[i].node_id, i + 1);
        EXPECT_FALSE(mgr.motors[i].enabled);
    }
}

TEST_F(MotorManagerTest, Init_NullGuards) {
    EXPECT_FALSE(motor_manager_init(NULL, motors, 4, &protocol));
    EXPECT_FALSE(motor_manager_init(&mgr, NULL, 4, &protocol));
    EXPECT_FALSE(motor_manager_init(&mgr, motors, 0, &protocol));
    EXPECT_FALSE(motor_manager_init(&mgr, motors, 4, NULL));
}

TEST_F(MotorManagerTest, EnableDisableAll) {
    motor_manager_init(&mgr, motors, 4, &protocol);

    EXPECT_TRUE(motor_manager_enable_all(&mgr));
    for (int i = 0; i < 4; i++) {
        EXPECT_TRUE(mgr.motors[i].enabled);
    }

    EXPECT_TRUE(motor_manager_disable_all(&mgr));
    for (int i = 0; i < 4; i++) {
        EXPECT_FALSE(mgr.motors[i].enabled);
    }
}

TEST_F(MotorManagerTest, EnableDisableAll_NullGuards) {
    EXPECT_FALSE(motor_manager_enable_all(NULL));
    EXPECT_FALSE(motor_manager_disable_all(NULL));

    mgr.motors = NULL;
    EXPECT_FALSE(motor_manager_enable_all(&mgr));
    EXPECT_FALSE(motor_manager_disable_all(&mgr));
}

TEST_F(MotorManagerTest, SetTargetPosition_Valid) {
    motor_manager_init(&mgr, motors, 4, &protocol);

    EXPECT_TRUE(motor_manager_set_target_position(&mgr, 0, 1000));
    EXPECT_EQ(mgr.motors[0].target_position, 1000);

    EXPECT_TRUE(motor_manager_set_target_position(&mgr, 3, -500));
    EXPECT_EQ(mgr.motors[3].target_position, -500);
}

TEST_F(MotorManagerTest, SetTargetPosition_OutOfRange) {
    motor_manager_init(&mgr, motors, 4, &protocol);

    EXPECT_FALSE(motor_manager_set_target_position(&mgr, 4, 1000));
    EXPECT_FALSE(motor_manager_set_target_position(NULL, 0, 1000));
}

TEST_F(MotorManagerTest, Update_Success) {
    motor_manager_init(&mgr, motors, 4, &protocol);
    motor_manager_enable_all(&mgr);
    motor_manager_set_target_position(&mgr, 0, 2000);

    EXPECT_TRUE(motor_manager_update(&mgr));
    EXPECT_EQ(mgr.motors[0].fsm.current_state, CIA402_STATE_OPERATION_ENABLED);
    EXPECT_EQ(mgr.motors[0].actual_position, 0);
}

TEST_F(MotorManagerTest, Update_NullGuards) {
    EXPECT_FALSE(motor_manager_update(NULL));

    mgr.motors = NULL;
    EXPECT_FALSE(motor_manager_update(&mgr));
}
