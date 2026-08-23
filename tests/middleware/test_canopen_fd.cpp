#include <gtest/gtest.h>
#include "canopen_node.h"
#include "can_mock.h"

TEST(CANopenFDTest, NodeInitFDConfig) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_can_config_t config;
    config.nominal_baudrate = 1000000;
    config.data_baudrate = 2000000; // 2 Mbps requested
    config.fd_enabled = true;

    canopen_node_t node;
    EXPECT_FALSE(canopen_node_init(NULL, 1, &mock, &config));
    EXPECT_FALSE(canopen_node_init(&node, 1, NULL, &config));

    // Test with explicit config
    EXPECT_TRUE(canopen_node_init(&node, 1, &mock, &config));
    EXPECT_EQ(node.config.data_baudrate, 2000000);
    EXPECT_TRUE(node.config.fd_enabled);

    // Test with NULL config (default 2Mbps FD config used)
    canopen_node_t node_default;
    EXPECT_TRUE(canopen_node_init(&node_default, 2, &mock, NULL));
    EXPECT_EQ(node_default.config.data_baudrate, 2000000);
    EXPECT_TRUE(node_default.config.fd_enabled);
}

TEST(CANopenFDTest, LargePayloadSDOWrite) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_sdo_t sdo;
    EXPECT_TRUE(canopen_sdo_init(&sdo, 1, &mock, true)); // FD enabled

    uint8_t payload[32];
    for (int i = 0; i < 32; i++) payload[i] = (uint8_t)i;

    EXPECT_TRUE(canopen_sdo_write_expedited(&sdo, 0x2000, 1, payload, 32));
}

static bool rx_nmt_mock(can_interface_t *self, can_frame_t *frame) {
    (void)self;
    if (!frame) return false;
    frame->id = CANOPEN_COB_NMT;
    frame->len = 2;
    frame->data[0] = CANOPEN_NMT_CMD_START_NODE;
    frame->data[1] = 1;
    return true;
}

static bool rx_pdo_mock(can_interface_t *self, can_frame_t *frame) {
    (void)self;
    if (!frame) return false;
    frame->id = CANOPEN_COB_RPDO1_BASE + 1; // 0x201
    frame->len = 2;
    frame->data[0] = 0x0F;
    frame->data[1] = 0x00;
    return true;
}

static bool rx_sdo_mock(can_interface_t *self, can_frame_t *frame) {
    (void)self;
    if (!frame) return false;
    frame->id = CANOPEN_COB_TSDO_BASE + 1; // 0x581
    frame->len = 8;
    frame->data[0] = 0x4F; // SDO Upload Response 2 bytes
    frame->data[1] = 0x41;
    frame->data[2] = 0x60; // Index 0x6041
    frame->data[3] = 0x00;
    frame->data[4] = 0x27;
    frame->data[5] = 0x00;
    return true;
}

TEST(CANopenFDTest, NodeProcessLoopMsgProcessing) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_node_t node;
    canopen_node_init(&node, 1, &mock, NULL);

    // Null guard
    EXPECT_FALSE(canopen_node_process(NULL));

    // Mock returns no frame, process returns false
    EXPECT_FALSE(canopen_node_process(&node));

    // Mock returns NMT frame
    mock.receive = rx_nmt_mock;
    EXPECT_TRUE(canopen_node_process(&node));
    EXPECT_EQ(node.nmt.state, CANOPEN_NMT_STATE_OPERATIONAL);

    // Mock returns PDO frame
    mock.receive = rx_pdo_mock;
    EXPECT_TRUE(canopen_node_process(&node));

    // Mock returns SDO frame
    mock.receive = rx_sdo_mock;
    EXPECT_TRUE(canopen_node_process(&node));
}
