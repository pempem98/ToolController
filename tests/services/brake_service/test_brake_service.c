#include "unity.h"
#include "brake_service.h"
#include <string.h>

static bool s_mock_engaged = true;

static status_t mock_init(brake_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_engage(brake_interface_t *self) { (void)self; s_mock_engaged = true; return STATUS_OK; }
static status_t mock_release(brake_interface_t *self) { (void)self; s_mock_engaged = false; return STATUS_OK; }
static status_t mock_is_engaged(brake_interface_t *self, bool *is_en) {
    (void)self;
    if (is_en) *is_en = s_mock_engaged;
    return STATUS_OK;
}

static brake_interface_t s_mock_hw_brake;
static brake_service_t s_brake;

void test_brake_service_setUp(void) {
    memset(&s_mock_hw_brake, 0, sizeof(s_mock_hw_brake));
    s_mock_hw_brake.init = mock_init;
    s_mock_hw_brake.engage = mock_engage;
    s_mock_hw_brake.release = mock_release;
    s_mock_hw_brake.is_engaged = mock_is_engaged;
    s_mock_engaged = true;

    memset(&s_brake, 0, sizeof(s_brake));
}

void test_brake_service_tearDown(void) {
}

void test_brake_service_init_success(void) {
    TEST_ASSERT_TRUE(brake_service_init(&s_brake, &s_mock_hw_brake, 100));
    TEST_ASSERT_EQUAL_PTR(&s_mock_hw_brake, s_brake.hw_brake);
    TEST_ASSERT_EQUAL_UINT32(100, s_brake.delay_ms);
    TEST_ASSERT_EQUAL(BRAKE_STATE_ENGAGED, s_brake.state);
    TEST_ASSERT_TRUE(s_mock_engaged);
}

void test_brake_service_init_null_guards(void) {
    TEST_ASSERT_FALSE(brake_service_init(NULL, &s_mock_hw_brake, 100));
    TEST_ASSERT_FALSE(brake_service_init(&s_brake, NULL, 100));
}

void test_brake_service_release_success(void) {
    brake_service_init(&s_brake, &s_mock_hw_brake, 100);

    TEST_ASSERT_TRUE(brake_service_release(&s_brake));
    TEST_ASSERT_EQUAL(BRAKE_STATE_RELEASED, s_brake.state);
    TEST_ASSERT_FALSE(s_mock_engaged);

    TEST_ASSERT_FALSE(brake_service_release(NULL));
}

void test_brake_service_engage_success(void) {
    brake_service_init(&s_brake, &s_mock_hw_brake, 100);
    brake_service_release(&s_brake);

    TEST_ASSERT_TRUE(brake_service_engage(&s_brake));
    TEST_ASSERT_EQUAL(BRAKE_STATE_ENGAGED, s_brake.state);
    TEST_ASSERT_TRUE(s_mock_engaged);

    TEST_ASSERT_FALSE(brake_service_engage(NULL));
}

void test_brake_service_get_state(void) {
    brake_service_init(&s_brake, &s_mock_hw_brake, 100);
    TEST_ASSERT_EQUAL(BRAKE_STATE_ENGAGED, brake_service_get_state(&s_brake));

    brake_service_release(&s_brake);
    TEST_ASSERT_EQUAL(BRAKE_STATE_RELEASED, brake_service_get_state(&s_brake));
}


