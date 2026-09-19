#include "unity.h"
#include "brake_driver.h"
#include "stm32_hal_stub.h"
#include <string.h>

static brake_interface_t s_brake;
static stm32_brake_priv_t s_priv;

void test_brake_driver_setUp(void) {
    hal_stub_reset();
    memset(&s_brake, 0, sizeof(s_brake));
    memset(&s_priv, 0, sizeof(s_priv));
    stm32_brake_driver_create(&s_brake, &s_priv, EN_GPIO_Port, EN_Pin);
}

void test_brake_driver_tearDown(void) {
}

void test_brake_create_populates_interface(void) {
    TEST_ASSERT_NOT_NULL(s_brake.init);
    TEST_ASSERT_NOT_NULL(s_brake.engage);
    TEST_ASSERT_NOT_NULL(s_brake.release);
    TEST_ASSERT_NOT_NULL(s_brake.is_engaged);
    TEST_ASSERT_EQUAL_PTR(&s_priv, s_brake.priv_data);
    TEST_ASSERT_TRUE(s_priv.is_currently_engaged);
}

void test_brake_init_engages_brake(void) {
    s_priv.is_currently_engaged = false;
    hal_stub_set_pin_state(EN_GPIO_Port, EN_Pin, GPIO_PIN_SET);

    TEST_ASSERT_EQUAL(STATUS_OK, s_brake.init(&s_brake));
    TEST_ASSERT_TRUE(s_priv.is_currently_engaged);
    // Engage writes GPIO_PIN_RESET to safely lock
    TEST_ASSERT_EQUAL(GPIO_PIN_RESET, hal_stub_get_pin_state(EN_GPIO_Port, EN_Pin));
}

void test_brake_release_sets_pin_high(void) {
    s_brake.init(&s_brake);

    TEST_ASSERT_EQUAL(STATUS_OK, s_brake.release(&s_brake));
    TEST_ASSERT_FALSE(s_priv.is_currently_engaged);
    TEST_ASSERT_EQUAL(GPIO_PIN_SET, hal_stub_get_pin_state(EN_GPIO_Port, EN_Pin));

    bool is_en = true;
    TEST_ASSERT_EQUAL(STATUS_OK, s_brake.is_engaged(&s_brake, &is_en));
    TEST_ASSERT_FALSE(is_en);
}

void test_brake_engage_sets_pin_low(void) {
    s_brake.release(&s_brake);

    TEST_ASSERT_EQUAL(STATUS_OK, s_brake.engage(&s_brake));
    TEST_ASSERT_TRUE(s_priv.is_currently_engaged);
    TEST_ASSERT_EQUAL(GPIO_PIN_RESET, hal_stub_get_pin_state(EN_GPIO_Port, EN_Pin));

    bool is_en = false;
    TEST_ASSERT_EQUAL(STATUS_OK, s_brake.is_engaged(&s_brake, &is_en));
    TEST_ASSERT_TRUE(is_en);
}

void test_brake_null_guards(void) {
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_brake.init(NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_brake.engage(NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_brake.release(NULL));
    TEST_ASSERT_EQUAL(STATUS_INVALID_PARAM, s_brake.is_engaged(NULL, NULL));
}


