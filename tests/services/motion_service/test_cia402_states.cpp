#include <gtest/gtest.h>
#include "cia402_state_machine.h"

TEST(CiA402Test, InitDefaults) {
    cia402_handle_t fsm;
    cia402_init(&fsm);

    EXPECT_EQ(fsm.current_state, CIA402_STATE_SWITCH_ON_DISABLED);
    EXPECT_EQ(fsm.status_word, 0x0000);
    EXPECT_EQ(fsm.control_word, 0x0000);
}

TEST(CiA402Test, InitNullSafe) {
    cia402_init(NULL);
}

TEST(CiA402Test, StatusWordParsing) {
    EXPECT_EQ(cia402_parse_statusword(0x0000), CIA402_STATE_NOT_READY_TO_SWITCH_ON);
    EXPECT_EQ(cia402_parse_statusword(0x0240), CIA402_STATE_SWITCH_ON_DISABLED);
    EXPECT_EQ(cia402_parse_statusword(0x0221), CIA402_STATE_READY_TO_SWITCH_ON);
    EXPECT_EQ(cia402_parse_statusword(0x0223), CIA402_STATE_SWITCHED_ON);
    EXPECT_EQ(cia402_parse_statusword(0x0227), CIA402_STATE_OPERATION_ENABLED);
    EXPECT_EQ(cia402_parse_statusword(0x0207), CIA402_STATE_QUICK_STOP_ACTIVE);
    EXPECT_EQ(cia402_parse_statusword(0x020F), CIA402_STATE_FAULT_REACTION_ACTIVE);
    EXPECT_EQ(cia402_parse_statusword(0x0208), CIA402_STATE_FAULT);
    EXPECT_EQ(cia402_parse_statusword(0xFFFF), CIA402_STATE_NOT_READY_TO_SWITCH_ON);
}

TEST(CiA402Test, ControlWordGeneration) {
    cia402_handle_t fsm;
    cia402_init(&fsm);

    uint16_t cw = cia402_generate_controlword(&fsm, CIA402_CMD_SHUTDOWN);
    EXPECT_EQ(cw & 0x000F, 0x0006);

    cw = cia402_generate_controlword(&fsm, CIA402_CMD_SWITCH_ON);
    EXPECT_EQ(cw & 0x000F, 0x0007);

    cw = cia402_generate_controlword(&fsm, CIA402_CMD_ENABLE_OPERATION);
    EXPECT_EQ(cw & 0x000F, 0x000F);

    cw = cia402_generate_controlword(&fsm, CIA402_CMD_DISABLE_OPERATION);
    EXPECT_EQ(cw & 0x000F, 0x0007);

    cw = cia402_generate_controlword(&fsm, CIA402_CMD_DISABLE_VOLTAGE);
    EXPECT_EQ(cw & 0x0002, 0x0000);

    cw = cia402_generate_controlword(&fsm, CIA402_CMD_QUICK_STOP);
    EXPECT_EQ(cw & 0x0004, 0x0000);

    cw = cia402_generate_controlword(&fsm, CIA402_CMD_FAULT_RESET);
    EXPECT_EQ(cw & 0x0080, 0x0080);
}

TEST(CiA402Test, GenerateControlwordNullSafe) {
    EXPECT_EQ(cia402_generate_controlword(NULL, CIA402_CMD_SHUTDOWN), 0u);
}

TEST(CiA402Test, ProcessEvent) {
    cia402_handle_t fsm;
    cia402_init(&fsm);

    EXPECT_TRUE(cia402_process_event(&fsm, CIA402_CMD_ENABLE_OPERATION));
    EXPECT_EQ(fsm.control_word & 0x000F, 0x000F);
}

TEST(CiA402Test, ProcessEventNullSafe) {
    EXPECT_FALSE(cia402_process_event(NULL, CIA402_CMD_ENABLE_OPERATION));
}
