#include "unity.h"
#include "tmc2209_driver.h"
#include "stm32_hal_stub.h"
#include <string.h>

static motor_interface_t s_motor;
static tmc2209_motor_priv_t s_priv;

void test_tmc2209_driver_setUp(void) {
    hal_stub_reset();
    memset(&s_motor, 0, sizeof(s_motor));
    memset(&s_priv, 0, sizeof(s_priv));

    tmc2209_driver_create(&s_motor, &s_priv, 0,
                          STEP_1_GPIO_Port, STEP_1_Pin,
                          DIR_1_GPIO_Port, DIR_1_Pin,
                          EN_GPIO_Port, EN_Pin,
                          NULL, false);
}

void test_tmc2209_driver_tearDown(void) {
}

void test_tmc2209_create_populates_interface(void) {
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

void test_tmc2209_init_disables_motor_pin_high(void) {
    // TMC2209 Enable is active LOW -> disabled means pin is SET (HIGH)
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.init(&s_motor));
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, hal_stub_get_pin_state(EN_GPIO_Port, EN_Pin));
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, hal_stub_get_pin_state(DIR_1_GPIO_Port, DIR_1_Pin));
    TEST_ASSERT_FALSE(s_priv.enabled);
    TEST_ASSERT_EQUAL_INT32(0, s_priv.current_steps);
}

void test_tmc2209_enable_sets_pin_low(void) {
    s_motor.init(&s_motor);

    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.set_enabled(&s_motor, true));
    TEST_ASSERT_TRUE(s_priv.enabled);
    // Active LOW -> enabled means pin is RESET (LOW)
    TEST_ASSERT_EQUAL(GPIO_PIN_RESET, hal_stub_get_pin_state(EN_GPIO_Port, EN_Pin));

    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.set_enabled(&s_motor, false));
    TEST_ASSERT_FALSE(s_priv.enabled);
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, hal_stub_get_pin_state(EN_GPIO_Port, EN_Pin));
}

void test_tmc2209_set_direction_normal_and_inverted(void) {
    s_motor.init(&s_motor);

    // Normal direction (dir_inverted = false)
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.set_direction(&s_motor, MOTOR_DIR_CW));
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, hal_stub_get_pin_state(DIR_1_GPIO_Port, DIR_1_Pin));

    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.set_direction(&s_motor, MOTOR_DIR_CCW));
    TEST_ASSERT_EQUAL(GPIO_PIN_RESET, hal_stub_get_pin_state(DIR_1_GPIO_Port, DIR_1_Pin));

    // Test with inverted direction
    s_priv.dir_inverted = true;
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.set_direction(&s_motor, MOTOR_DIR_CW));
    TEST_ASSERT_EQUAL(GPIO_PIN_RESET, hal_stub_get_pin_state(DIR_1_GPIO_Port, DIR_1_Pin));

    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.set_direction(&s_motor, MOTOR_DIR_CCW));
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, hal_stub_get_pin_state(DIR_1_GPIO_Port, DIR_1_Pin));
}

void test_tmc2209_move_to_updates_position(void) {
    s_motor.init(&s_motor);

    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.move_to(&s_motor, 5000, 1000, MOTOR_DIR_CW));
    TEST_ASSERT_EQUAL_INT32(5000, s_priv.target_steps);
    TEST_ASSERT_EQUAL_INT32(5000, s_priv.current_steps);
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, hal_stub_get_pin_state(DIR_1_GPIO_Port, DIR_1_Pin));

    int32_t pos = 0;
    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.get_actual_position(&s_motor, &pos));
    TEST_ASSERT_EQUAL_INT32(5000, pos);
}

void test_tmc2209_home_resets_position(void) {
    s_priv.current_steps = 12345;
    s_priv.target_steps = 12345;

    TEST_ASSERT_EQUAL(STATUS_OK, s_motor.home(&s_motor));
    TEST_ASSERT_EQUAL_INT32(0, s_priv.current_steps);
    TEST_ASSERT_EQUAL_INT32(0, s_priv.target_steps);
}

void test_tmc2209_null_guards(void) {
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.init(NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.set_enabled(NULL, true));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.set_direction(NULL, MOTOR_DIR_CW));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.move_to(NULL, 0, 0, MOTOR_DIR_CW));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.rotate(NULL, MOTOR_DIR_CW, 0));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.get_actual_position(NULL, NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.home(NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_motor.stop(NULL));
}


