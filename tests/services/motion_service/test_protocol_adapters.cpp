#include <gtest/gtest.h>
#include "motor_protocol.h"

extern "C" {
    void canopen_adapter_create(motor_protocol_adapter_t *adapter);
    void ethercat_coe_adapter_create(motor_protocol_adapter_t *adapter);
    void canfd_adapter_create(motor_protocol_adapter_t *adapter);
}

TEST(ProtocolAdaptersTest, CANOpenAdapterFull) {
    motor_protocol_adapter_t adapter;
    
    // Null guard check
    canopen_adapter_create(NULL);

    canopen_adapter_create(&adapter);
    EXPECT_EQ(adapter.type, MOTOR_PROTOCOL_CANOPEN);
    EXPECT_NE(adapter.init, nullptr);
    EXPECT_NE(adapter.send_target_position, nullptr);
    EXPECT_NE(adapter.send_controlword, nullptr);
    EXPECT_NE(adapter.read_statusword, nullptr);
    EXPECT_NE(adapter.read_actual_position, nullptr);

    EXPECT_TRUE(adapter.init(&adapter));
    EXPECT_TRUE(adapter.send_target_position(&adapter, 1, 5000));
    EXPECT_TRUE(adapter.send_controlword(&adapter, 1, 0x000F));

    uint16_t status = 0;
    EXPECT_TRUE(adapter.read_statusword(&adapter, 1, &status));
    EXPECT_EQ(status, 0x0027);
    EXPECT_TRUE(adapter.read_statusword(&adapter, 1, NULL));

    int32_t pos = -1;
    EXPECT_TRUE(adapter.read_actual_position(&adapter, 1, &pos));
    EXPECT_EQ(pos, 0);
    EXPECT_TRUE(adapter.read_actual_position(&adapter, 1, NULL));
}

TEST(ProtocolAdaptersTest, EtherCATCoEAdapterFull) {
    motor_protocol_adapter_t adapter;

    // Null guard check
    ethercat_coe_adapter_create(NULL);

    ethercat_coe_adapter_create(&adapter);
    EXPECT_EQ(adapter.type, MOTOR_PROTOCOL_ETHERCAT_COE);
    EXPECT_NE(adapter.init, nullptr);
    EXPECT_NE(adapter.send_target_position, nullptr);
    EXPECT_NE(adapter.send_controlword, nullptr);
    EXPECT_NE(adapter.read_statusword, nullptr);
    EXPECT_NE(adapter.read_actual_position, nullptr);

    EXPECT_TRUE(adapter.init(&adapter));
    EXPECT_TRUE(adapter.send_target_position(&adapter, 1, 10000));
    EXPECT_TRUE(adapter.send_controlword(&adapter, 1, 0x000F));

    uint16_t status = 0;
    EXPECT_TRUE(adapter.read_statusword(&adapter, 1, &status));
    EXPECT_EQ(status, 0x0027);
    EXPECT_TRUE(adapter.read_statusword(&adapter, 1, NULL));

    int32_t pos = -1;
    EXPECT_TRUE(adapter.read_actual_position(&adapter, 1, &pos));
    EXPECT_EQ(pos, 0);
    EXPECT_TRUE(adapter.read_actual_position(&adapter, 1, NULL));
}

TEST(ProtocolAdaptersTest, CANFDAdapterNullGuard) {
    canfd_adapter_create(NULL);
}
