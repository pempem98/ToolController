#include <gtest/gtest.h>
#include "motor_manager.h"

extern "C" {
    void canfd_adapter_create(motor_protocol_adapter_t *adapter);
}

TEST(MotorManagerTest, Initialization) {
    motor_instance_t motors[4];
    motor_protocol_adapter_t protocol;
    canfd_adapter_create(&protocol);

    motor_manager_t mgr;
    EXPECT_TRUE(motor_manager_init(&mgr, motors, 4, &protocol));
    EXPECT_EQ(mgr.motor_count, 4);

    for (int i = 0; i < 4; i++) {
        EXPECT_EQ(mgr.motors[i].node_id, i + 1);
        EXPECT_FALSE(mgr.motors[i].enabled);
    }
}

TEST(MotorManagerTest, EnableDisableAll) {
    motor_instance_t motors[4];
    motor_protocol_adapter_t protocol;
    canfd_adapter_create(&protocol);

    motor_manager_t mgr;
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
