#include "unity.h"
#include "operator_input_driver.h"
#include "stm32_hal_stub.h"
#include <string.h>

static operator_input_interface_t s_input;
static uint16_t s_test_adc_buffer[2];

void test_operator_input_driver_setUp(void) {
    hal_stub_reset();
    memset(&s_input, 0, sizeof(s_input));
    s_test_adc_buffer[0] = 32768;
    s_test_adc_buffer[1] = 32768;
    operator_input_driver_set_buffer_override(s_test_adc_buffer);
    operator_input_driver_create(&s_input);
}

void test_operator_input_driver_tearDown(void) {
    operator_input_driver_set_buffer_override(NULL);
}

void test_operator_create_populates_interface(void) {
    TEST_ASSERT_NOT_NULL(s_input.init);
    TEST_ASSERT_NOT_NULL(s_input.read_axis);
    TEST_ASSERT_NOT_NULL(s_input.read_button);
}

void test_operator_compute_axis_norm_values(void) {
    // Center point: 32768 -> 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, operator_input_compute_axis_norm(32768));

    // Full positive: 65535 -> 0.999969... -> clamped <= 1.0f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, operator_input_compute_axis_norm(65535));

    // Full negative: 0 -> -1.0f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -1.0f, operator_input_compute_axis_norm(0));

    // Half positive: 32768 + 16384 = 49152 -> +0.5f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, operator_input_compute_axis_norm(49152));

    // Half negative: 32768 - 16384 = 16384 -> -0.5f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -0.5f, operator_input_compute_axis_norm(16384));
}

void test_operator_read_axis_uses_buffer(void) {
    s_input.init(&s_input);

    s_test_adc_buffer[0] = 49152; // Axis 0 = +0.5f
    s_test_adc_buffer[1] = 16384; // Axis 1 = -0.5f

    float val0 = 0.0f, val1 = 0.0f, val2 = 99.0f;
    TEST_ASSERT_EQUAL(STATUS_OK, s_input.read_axis(&s_input, OPERATOR_AXIS_0, &val0));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.5f, val0);

    TEST_ASSERT_EQUAL(STATUS_OK, s_input.read_axis(&s_input, OPERATOR_AXIS_1, &val1));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, -0.5f, val1);

    // Unused axes return 0.0f
    TEST_ASSERT_EQUAL(STATUS_OK, s_input.read_axis(&s_input, OPERATOR_AXIS_2, &val2));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, val2);
}

void test_operator_read_button_brake_active_low(void) {
    s_input.init(&s_input);

    // JOY_SW is active LOW: RESET = pressed, SET = released
    hal_stub_set_pin_state(JOY_SW_GPIO_Port, JOY_SW_Pin, GPIO_PIN_RESET);
    bool pressed = false;
    TEST_ASSERT_EQUAL(STATUS_OK, s_input.read_button(&s_input, OPERATOR_BTN_BRAKE, &pressed));
    TEST_ASSERT_TRUE(pressed);

    hal_stub_set_pin_state(JOY_SW_GPIO_Port, JOY_SW_Pin, GPIO_PIN_SET);
    pressed = true;
    TEST_ASSERT_EQUAL(STATUS_OK, s_input.read_button(&s_input, OPERATOR_BTN_BRAKE, &pressed));
    TEST_ASSERT_FALSE(pressed);

    // Unused buttons return false
    bool other_btn = true;
    TEST_ASSERT_EQUAL(STATUS_OK, s_input.read_button(&s_input, 1, &other_btn));
    TEST_ASSERT_FALSE(other_btn);
}

void test_operator_null_guards(void) {
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_input.init(NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_input.read_axis(NULL, 0, NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_input.read_axis(&s_input, 99, NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_input.read_button(NULL, 0, NULL));
}


