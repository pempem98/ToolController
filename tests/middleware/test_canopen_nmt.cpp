#include <gtest/gtest.h>
#include "canopen_nmt.h"
#include "can_mock.h"

TEST(CANopenNMTTest, InitStateIsPreOp) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_nmt_t nmt;
    EXPECT_FALSE(canopen_nmt_init(NULL, 1, &mock, false));
    EXPECT_TRUE(canopen_nmt_init(&nmt, 1, &mock, false));
    EXPECT_EQ(canopen_nmt_get_state(&nmt), CANOPEN_NMT_STATE_PRE_OPERATIONAL);
    EXPECT_EQ(canopen_nmt_get_state(NULL), CANOPEN_NMT_STATE_INITIALIZING);
}

TEST(CANopenNMTTest, SendStartNodeCommand) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_nmt_t nmt;
    canopen_nmt_init(&nmt, 1, &mock, false);

    EXPECT_FALSE(canopen_nmt_send_command(NULL, 1, CANOPEN_NMT_CMD_START_NODE));
    EXPECT_TRUE(canopen_nmt_send_command(&nmt, 1, CANOPEN_NMT_CMD_START_NODE));
}

TEST(CANopenNMTTest, ProcessRxNMTMsgTransitions) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_nmt_t nmt;
    canopen_nmt_init(&nmt, 1, &mock, false);

    // Null guards
    can_frame_t frame;
    frame.id = CANOPEN_COB_NMT;
    frame.len = 2;
    frame.data[0] = CANOPEN_NMT_CMD_START_NODE;
    frame.data[1] = 1;

    EXPECT_FALSE(canopen_nmt_process_msg(NULL, &frame));
    EXPECT_FALSE(canopen_nmt_process_msg(&nmt, NULL));

    // START
    EXPECT_TRUE(canopen_nmt_process_msg(&nmt, &frame));
    EXPECT_EQ(canopen_nmt_get_state(&nmt), CANOPEN_NMT_STATE_OPERATIONAL);

    // STOP
    frame.data[0] = CANOPEN_NMT_CMD_STOP_NODE;
    EXPECT_TRUE(canopen_nmt_process_msg(&nmt, &frame));
    EXPECT_EQ(canopen_nmt_get_state(&nmt), CANOPEN_NMT_STATE_STOPPED);

    // ENTER PRE-OP
    frame.data[0] = CANOPEN_NMT_CMD_ENTER_PRE_OP;
    EXPECT_TRUE(canopen_nmt_process_msg(&nmt, &frame));
    EXPECT_EQ(canopen_nmt_get_state(&nmt), CANOPEN_NMT_STATE_PRE_OPERATIONAL);

    // RESET NODE
    frame.data[0] = CANOPEN_NMT_CMD_RESET_NODE;
    EXPECT_TRUE(canopen_nmt_process_msg(&nmt, &frame));
    EXPECT_EQ(canopen_nmt_get_state(&nmt), CANOPEN_NMT_STATE_INITIALIZING);

    // RESET COMM
    frame.data[0] = CANOPEN_NMT_CMD_RESET_COMM;
    EXPECT_TRUE(canopen_nmt_process_msg(&nmt, &frame));
    EXPECT_EQ(canopen_nmt_get_state(&nmt), CANOPEN_NMT_STATE_INITIALIZING);

    // Unknown command
    frame.data[0] = 0xFF;
    EXPECT_FALSE(canopen_nmt_process_msg(&nmt, &frame));

    // Frame not NMT (e.g. id != 0)
    frame.id = 0x100;
    EXPECT_FALSE(canopen_nmt_process_msg(&nmt, &frame));

    // Frame for different node
    frame.id = CANOPEN_COB_NMT;
    frame.data[0] = CANOPEN_NMT_CMD_START_NODE;
    frame.data[1] = 99; // Not node 1
    EXPECT_FALSE(canopen_nmt_process_msg(&nmt, &frame));
}

TEST(CANopenNMTTest, SendHeartbeatFrame) {
    can_interface_t mock;
    can_mock_create(&mock);

    canopen_nmt_t nmt;
    EXPECT_FALSE(canopen_nmt_send_heartbeat(NULL));
    canopen_nmt_init(&nmt, 1, &mock, true); // FD enabled

    EXPECT_TRUE(canopen_nmt_send_heartbeat(&nmt));
}
