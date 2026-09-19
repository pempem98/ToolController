#include "unity.h"
#include "bldc_can_driver.h"
#include <string.h>

static can_interface_t s_mock_can;
static can_frame_t s_last_sent_frame;
static bool s_mock_send_result = true;
static int s_mock_send_call_count = 0;

static bool mock_can_send(can_interface_t *self, const can_frame_t *frame) {
    (void)self;
    s_mock_send_call_count++;
    if (frame) {
        memcpy(&s_last_sent_frame, frame, sizeof(can_frame_t));
    }
    return s_mock_send_result;
}

static motor_interface_t s_motor;
static bldc_can_motor_priv_t s_priv;

void test_bldc_can_driver_setUp(void) {
    memset(&s_mock_can, 0, sizeof(s_mock_can));
    s_mock_can.send = mock_can_send;
    s_mock_send_result = true;
    s_mock_send_call_count = 0;
    memset(&s_last_sent_frame, 0, sizeof(s_last_sent_frame));

    memset(&s_motor, 0, sizeof(s_motor));
    memset(&s_priv, 0, sizeof(s_priv));
    bldc_can_driver_create(&s_motor, &s_priv, 1, &s_mock_can);
}

void test_bldc_driver_create_populates_interface(void) {
    TEST_ASSERT_NOT_NULL(s_motor.init);
    TEST_ASSERT_NOT_NULL(s_motor.set_enabled);
    TEST_ASSERT_NOT_NULL(s_motor.set_direction);
    TEST_ASSERT_NOT_NULL(s_motor.move_to);
    TEST_ASSERT_NOT_NULL(s_motor.rotate);
    TEST_ASSERT_NOT_NULL(s_motor.get_actual_position);
    TEST_ASSERT_NOT_NULL(s_motor.get_encoder_ticks);
    TEST_ASSERT_NOT_NULL(s_motor.home);
    TEST_ASSERT_NOT_NULL(s_motor.stop);
    TEST_ASSERT_EQUAL_PTR(&s_priv, s_motor.priv_data);
}

void test_bldc_driver_init_resets_state(void) {
    s_priv.actual_position = 1000;
    s_priv.actual_speed = 500;
    s_priv.enabled = true;

    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.init(&s_motor));
    TEST_ASSERT_EQUAL_INT32(0, s_priv.actual_position);
    TEST_ASSERT_EQUAL_INT16(0, s_priv.actual_speed);
    TEST_ASSERT_FALSE(s_priv.enabled);
}

void test_bldc_driver_set_enabled_true_packs_enable_command(void) {
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.set_enabled(&s_motor, true));
    TEST_ASSERT_TRUE(s_priv.enabled);
    TEST_ASSERT_EQUAL_INT(1, s_mock_send_call_count);
    TEST_ASSERT_EQUAL_HEX32(0x141, s_last_sent_frame.id); // 0x140 + node_id(1)
    TEST_ASSERT_EQUAL_UINT8(0x88, s_last_sent_frame.data[0]); // BLDC_CMD_ENABLE_RUN
    TEST_ASSERT_EQUAL_UINT8(8, s_last_sent_frame.len);
}

void test_bldc_driver_set_enabled_false_packs_disable_command(void) {
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.set_enabled(&s_motor, false));
    TEST_ASSERT_FALSE(s_priv.enabled);
    TEST_ASSERT_EQUAL_HEX32(0x141, s_last_sent_frame.id);
    TEST_ASSERT_EQUAL_UINT8(0x89, s_last_sent_frame.data[0]); // BLDC_CMD_DISABLE_STOP
}

void test_bldc_driver_set_direction_updates_priv(void) {
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.set_direction(&s_motor, MOTOR_DIR_CCW));
    TEST_ASSERT_EQUAL(MOTOR_DIR_CCW, s_priv.current_dir);
}

void test_bldc_driver_move_to_packs_position_and_speed(void) {
    int32_t target = 0x12345678;
    uint32_t speed = 0x0A0B;

    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.move_to(&s_motor, target, speed, MOTOR_DIR_CW));
    TEST_ASSERT_EQUAL_INT32(target, s_priv.target_position);
    TEST_ASSERT_EQUAL_HEX32(0x141, s_last_sent_frame.id);
    TEST_ASSERT_EQUAL_UINT8(0xA4, s_last_sent_frame.data[0]); // BLDC_CMD_POS_CTRL
    TEST_ASSERT_EQUAL_UINT8(MOTOR_DIR_CW, s_last_sent_frame.data[1]);
    TEST_ASSERT_EQUAL_UINT8(0x0B, s_last_sent_frame.data[2]);
    TEST_ASSERT_EQUAL_UINT8(0x0A, s_last_sent_frame.data[3]);
    TEST_ASSERT_EQUAL_UINT8(0x78, s_last_sent_frame.data[4]);
    TEST_ASSERT_EQUAL_UINT8(0x56, s_last_sent_frame.data[5]);
    TEST_ASSERT_EQUAL_UINT8(0x34, s_last_sent_frame.data[6]);
    TEST_ASSERT_EQUAL_UINT8(0x12, s_last_sent_frame.data[7]);
}

void test_bldc_driver_rotate_packs_speed_and_direction(void) {
    uint32_t speed = 2000;
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.rotate(&s_motor, MOTOR_DIR_CW, speed));
    TEST_ASSERT_EQUAL_HEX32(0x141, s_last_sent_frame.id);
    TEST_ASSERT_EQUAL_UINT8(0xA2, s_last_sent_frame.data[0]); // BLDC_CMD_SPEED_CTRL
    TEST_ASSERT_EQUAL_UINT8(MOTOR_DIR_CW, s_last_sent_frame.data[1]);

    int32_t packed_speed = (int32_t)(s_last_sent_frame.data[4] |
                                    (s_last_sent_frame.data[5] << 8) |
                                    (s_last_sent_frame.data[6] << 16) |
                                    (s_last_sent_frame.data[7] << 24));
    TEST_ASSERT_EQUAL_INT32(2000, packed_speed);
}

void test_bldc_driver_home_sends_zero_command(void) {
    s_priv.actual_position = 5000;
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.home(&s_motor));
    TEST_ASSERT_EQUAL_INT32(0, s_priv.actual_position);
    TEST_ASSERT_EQUAL_UINT8(0x64, s_last_sent_frame.data[0]); // BLDC_CMD_SET_ZERO
}

void test_bldc_driver_stop_sends_emergency_stop(void) {
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.stop(&s_motor));
    TEST_ASSERT_EQUAL_UINT8(0x80, s_last_sent_frame.data[0]); // BLDC_CMD_EMG_STOP
}

void test_bldc_driver_get_position_and_ticks(void) {
    s_priv.actual_position = 4242;
    s_priv.encoder_ticks = 8484;

    int32_t pos = 0, ticks = 0;
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.get_actual_position(&s_motor, &pos));
    TEST_ASSERT_EQUAL_INT32(4242, pos);

    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.get_encoder_ticks(&s_motor, &ticks));
    TEST_ASSERT_EQUAL_INT32(8484, ticks);
}

void test_bldc_driver_process_rx_updates_state(void) {
    can_frame_t rx_frame;
    memset(&rx_frame, 0, sizeof(rx_frame));
    rx_frame.id = 0x241; // BLDC_CAN_BASE_RX_ID + node_id(1)
    rx_frame.data[0] = 0x9C; // BLDC_CMD_READ_STATUS
    rx_frame.data[2] = 0xE8; // speed low byte (-1000 = 0xFC18, or 1000 = 0x03E8)
    rx_frame.data[3] = 0x03; // speed high byte -> 1000
    rx_frame.data[4] = 0x10; // pos byte 0
    rx_frame.data[5] = 0x27; // pos byte 1 -> 10000 = 0x2710
    rx_frame.data[6] = 0x00;
    rx_frame.data[7] = 0x00;

    bldc_can_driver_process_rx(&s_motor, &rx_frame);

    TEST_ASSERT_EQUAL_INT16(1000, s_priv.actual_speed);
    TEST_ASSERT_EQUAL_INT32(10000, s_priv.actual_position);
    TEST_ASSERT_EQUAL_INT32(10000, s_priv.encoder_ticks);
}

void test_bldc_driver_process_rx_ignores_wrong_node_id(void) {
    s_priv.actual_position = 0;
    can_frame_t rx_frame;
    memset(&rx_frame, 0, sizeof(rx_frame));
    rx_frame.id = 0x242; // Wrong node ID 2
    rx_frame.data[0] = 0x9C;
    rx_frame.data[4] = 0xFF;

    bldc_can_driver_process_rx(&s_motor, &rx_frame);
    TEST_ASSERT_EQUAL_INT32(0, s_priv.actual_position);
}

void test_bldc_driver_null_guards(void) {
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.init(NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.set_enabled(NULL, true));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.set_direction(NULL, MOTOR_DIR_CW));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.move_to(NULL, 0, 0, MOTOR_DIR_CW));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.rotate(NULL, MOTOR_DIR_CW, 0));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.get_actual_position(NULL, NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.home(NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.stop(NULL));
}


