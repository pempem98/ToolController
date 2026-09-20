#include "unity.h"
#include "cli_protocol.h"
#include <string.h>

void test_cli_protocol_setUp(void) {}
void test_cli_protocol_tearDown(void) {}

void test_cli_parse_help(void) {
    cli_command_t cmd;
    TEST_ASSERT_TRUE(cli_protocol_parse("help\r\n", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_HELP, cmd.type);

    TEST_ASSERT_TRUE(cli_protocol_parse("?", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_HELP, cmd.type);
}

void test_cli_parse_status(void) {
    cli_command_t cmd;
    TEST_ASSERT_TRUE(cli_protocol_parse("status", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_STATUS, cmd.type);
}

void test_cli_parse_stop_all(void) {
    cli_command_t cmd;
    TEST_ASSERT_TRUE(cli_protocol_parse("stop all", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_STOP_ALL, cmd.type);
}

void test_cli_parse_brake_on_off(void) {
    cli_command_t cmd;
    TEST_ASSERT_TRUE(cli_protocol_parse("brake on", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_BRAKE, cmd.type);
    TEST_ASSERT_TRUE(cmd.args.brake_engage);

    TEST_ASSERT_TRUE(cli_protocol_parse("brake off", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_BRAKE, cmd.type);
    TEST_ASSERT_FALSE(cmd.args.brake_engage);
}

void test_cli_parse_rotate_single_axis(void) {
    cli_command_t cmd;
    TEST_ASSERT_TRUE(cli_protocol_parse("rotate 1 cw 2000", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_ROTATE, cmd.type);
    TEST_ASSERT_EQUAL_UINT8(1, cmd.args.rotate.axis);
    TEST_ASSERT_EQUAL(MOTOR_DIR_CW, cmd.args.rotate.dir);
    TEST_ASSERT_EQUAL_UINT32(2000, cmd.args.rotate.speed);

    TEST_ASSERT_TRUE(cli_protocol_parse("rotate 2 ccw 1500", &cmd));
    TEST_ASSERT_EQUAL(MOTOR_DIR_CCW, cmd.args.rotate.dir);
    TEST_ASSERT_EQUAL_UINT32(1500, cmd.args.rotate.speed);
}

void test_cli_parse_move_single_axis(void) {
    cli_command_t cmd;
    TEST_ASSERT_TRUE(cli_protocol_parse("move 0 cw 1000 500", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_MOVE, cmd.type);
    TEST_ASSERT_EQUAL_UINT8(0, cmd.args.move.axis);
    TEST_ASSERT_EQUAL(MOTOR_DIR_CW, cmd.args.move.dir);
    TEST_ASSERT_EQUAL_INT32(1000, cmd.args.move.pos);
    TEST_ASSERT_EQUAL_UINT32(500, cmd.args.move.speed);
}

void test_cli_parse_rotate_all(void) {
    cli_command_t cmd;
    TEST_ASSERT_TRUE(cli_protocol_parse("rotate all cw 500 cw 600 ccw 700 cw 800", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_ROTATE_ALL, cmd.type);
    TEST_ASSERT_EQUAL(MOTOR_DIR_CW, cmd.args.rotate_all.dirs[0]);
    TEST_ASSERT_EQUAL_UINT32(500, cmd.args.rotate_all.speeds[0]);
    TEST_ASSERT_EQUAL(MOTOR_DIR_CCW, cmd.args.rotate_all.dirs[2]);
    TEST_ASSERT_EQUAL_UINT32(700, cmd.args.rotate_all.speeds[2]);
}

void test_cli_parse_move_all(void) {
    cli_command_t cmd;
    TEST_ASSERT_TRUE(cli_protocol_parse("move all 100 200 300 400 1000", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_MOVE_ALL, cmd.type);
    TEST_ASSERT_EQUAL_INT32(100, cmd.args.move_all.positions[0]);
    TEST_ASSERT_EQUAL_INT32(400, cmd.args.move_all.positions[3]);
    TEST_ASSERT_EQUAL_UINT32(1000, cmd.args.move_all.speed);
}

void test_cli_parse_log_on_off(void) {
    cli_command_t cmd;
    TEST_ASSERT_TRUE(cli_protocol_parse("log on", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_LOG, cmd.type);
    TEST_ASSERT_TRUE(cmd.args.log_enable);

    TEST_ASSERT_TRUE(cli_protocol_parse("log off", &cmd));
    TEST_ASSERT_FALSE(cmd.args.log_enable);
}

void test_cli_format_telemetry(void) {
    char buf[128];
    size_t len = cli_protocol_format_telemetry(0.5f, -0.25f, true, 100, 200, buf, sizeof(buf));
    TEST_ASSERT_TRUE(len > 0);
    TEST_ASSERT_NOT_NULL(strstr(buf, "TLM"));
    TEST_ASSERT_NOT_NULL(strstr(buf, "BRK=1"));
}

void test_cli_parse_null_and_invalid(void) {
    cli_command_t cmd;
    TEST_ASSERT_FALSE(cli_protocol_parse(NULL, &cmd));
    TEST_ASSERT_FALSE(cli_protocol_parse("help", NULL));
    TEST_ASSERT_FALSE(cli_protocol_parse("invalid_command_xyz", &cmd));
}

void test_cli_parse_empty_and_whitespace_only(void) {
    cli_command_t cmd;
    TEST_ASSERT_FALSE(cli_protocol_parse("", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_UNKNOWN, cmd.type);

    TEST_ASSERT_FALSE(cli_protocol_parse("   \t  ", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_UNKNOWN, cmd.type);
}

void test_cli_parse_too_long_string(void) {
    cli_command_t cmd;
    char long_str[200];
    memset(long_str, 'a', sizeof(long_str) - 1);
    long_str[sizeof(long_str) - 1] = '\0';

    TEST_ASSERT_FALSE(cli_protocol_parse(long_str, &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_UNKNOWN, cmd.type);
}

void test_cli_parse_brake_invalid_arg(void) {
    cli_command_t cmd;
    TEST_ASSERT_FALSE(cli_protocol_parse("brake foo", &cmd));
}

void test_cli_parse_log_invalid_arg(void) {
    cli_command_t cmd;
    TEST_ASSERT_FALSE(cli_protocol_parse("log foo", &cmd));
}

void test_cli_parse_rotate_all_numeric_fallback_format(void) {
    cli_command_t cmd;
    /* Numeric fallback (lines ~98-107 of cli_protocol.c) only triggers when the
     * primary "%15s %u ..." x4 scan does NOT consume all 8 fields (parsed != 8).
     * With exactly 4 numeric tokens, the primary scan yields parsed==4 (falls
     * through), and the numeric-only scan yields parsed_nums==4 (>=4 -> success),
     * filling only speeds/dirs[0..1] as <speed0> <dir0> <speed1> <dir1>; indices
     * 2-3 stay at their zero-initialized defaults (speed 0, dir CW). */
    TEST_ASSERT_TRUE(cli_protocol_parse("rotate_all 500 0 600 1", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_ROTATE_ALL, cmd.type);
    TEST_ASSERT_EQUAL_UINT32(500, cmd.args.rotate_all.speeds[0]);
    TEST_ASSERT_EQUAL(MOTOR_DIR_CW, cmd.args.rotate_all.dirs[0]);
    TEST_ASSERT_EQUAL_UINT32(600, cmd.args.rotate_all.speeds[1]);
    TEST_ASSERT_EQUAL(MOTOR_DIR_CCW, cmd.args.rotate_all.dirs[1]);
    TEST_ASSERT_EQUAL_UINT32(0, cmd.args.rotate_all.speeds[2]);
    TEST_ASSERT_EQUAL(MOTOR_DIR_CW, cmd.args.rotate_all.dirs[2]);
    TEST_ASSERT_EQUAL_UINT32(0, cmd.args.rotate_all.speeds[3]);
    TEST_ASSERT_EQUAL(MOTOR_DIR_CW, cmd.args.rotate_all.dirs[3]);
}

void test_cli_parse_rotate_all_insufficient_args_fails(void) {
    cli_command_t cmd;
    TEST_ASSERT_FALSE(cli_protocol_parse("rotate_all 1 2 3", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_UNKNOWN, cmd.type);
}

void test_cli_parse_move_all_insufficient_args_fails(void) {
    cli_command_t cmd;
    TEST_ASSERT_FALSE(cli_protocol_parse("move_all 1 2 3", &cmd));
}

void test_cli_parse_move_numeric_direction_format(void) {
    cli_command_t cmd;
    TEST_ASSERT_TRUE(cli_protocol_parse("move 0 100 500 1", &cmd));
    TEST_ASSERT_EQUAL(CLI_CMD_MOVE, cmd.type);
    TEST_ASSERT_EQUAL_UINT8(0, cmd.args.move.axis);
    TEST_ASSERT_EQUAL_INT32(100, cmd.args.move.pos);
    TEST_ASSERT_EQUAL_UINT32(500, cmd.args.move.speed);
    TEST_ASSERT_EQUAL(MOTOR_DIR_CCW, cmd.args.move.dir);

    TEST_ASSERT_TRUE(cli_protocol_parse("move 0 100 500 0", &cmd));
    TEST_ASSERT_EQUAL(MOTOR_DIR_CW, cmd.args.move.dir);
}

void test_cli_parse_move_numeric_direction_insufficient_args_fails(void) {
    cli_command_t cmd;
    TEST_ASSERT_FALSE(cli_protocol_parse("move 0 100", &cmd));
}

void test_cli_format_telemetry_buffer_too_small(void) {
    char buf[3];
    size_t len = cli_protocol_format_telemetry(0.5f, -0.25f, true, 100, 200, buf, sizeof(buf));
    TEST_ASSERT_EQUAL_UINT32(0, len);
}

void test_cli_format_telemetry_null_buffer(void) {
    char buf[128];
    TEST_ASSERT_EQUAL_UINT32(0, cli_protocol_format_telemetry(0.5f, -0.25f, true, 100, 200, NULL, sizeof(buf)));
    TEST_ASSERT_EQUAL_UINT32(0, cli_protocol_format_telemetry(0.5f, -0.25f, true, 100, 200, buf, 0));
}


