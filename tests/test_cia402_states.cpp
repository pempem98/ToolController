#include <gtest/gtest.h>
#include "cia402_state_machine.h"

TEST(CiA402Test, StatusWordParsing) {
    EXPECT_EQ(cia402_parse_statusword(0x0240), CIA402_STATE_SWITCH_ON_DISABLED);
    EXPECT_EQ(cia402_parse_statusword(0x0221), CIA402_STATE_READY_TO_SWITCH_ON);
    EXPECT_EQ(cia402_parse_statusword(0x0223), CIA402_STATE_SWITCHED_ON);
    EXPECT_EQ(cia402_parse_statusword(0x0227), CIA402_STATE_OPERATION_ENABLED);
    EXPECT_EQ(cia402_parse_statusword(0x0208), CIA402_STATE_FAULT);
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
}
