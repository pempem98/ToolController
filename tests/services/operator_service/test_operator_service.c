#include "unity.h"
#include "operator_service.h"
#include <string.h>

static float s_mock_axes[OPERATOR_MAX_AXES];
static bool  s_mock_buttons[OPERATOR_MAX_BUTTONS];

static status_t mock_init(operator_input_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_read_axis(operator_input_interface_t *self, uint8_t axis, float *val) {
    (void)self;
    if (!val || axis >= OPERATOR_MAX_AXES) return STATUS_INVALID_PARAM;
    *val = s_mock_axes[axis];
    return STATUS_OK;
}
static status_t mock_read_button(operator_input_interface_t *self, uint8_t btn, bool *pressed) {
    (void)self;
    if (!pressed || btn >= OPERATOR_MAX_BUTTONS) return STATUS_INVALID_PARAM;
    *pressed = s_mock_buttons[btn];
    return STATUS_OK;
}

static operator_input_interface_t s_mock_input;
static operator_service_t s_svc;

void test_operator_service_setUp(void) {
    memset(&s_mock_input, 0, sizeof(s_mock_input));
    s_mock_input.init = mock_init;
    s_mock_input.read_axis = mock_read_axis;
    s_mock_input.read_button = mock_read_button;

    for (int i = 0; i < OPERATOR_MAX_AXES; i++) s_mock_axes[i] = 0.0f;
    for (int i = 0; i < OPERATOR_MAX_BUTTONS; i++) s_mock_buttons[i] = false;

    memset(&s_svc, 0, sizeof(s_svc));
}

void test_operator_service_tearDown(void) {
}

void test_operator_service_init_success(void) {
    TEST_ASSERT_TRUE(operator_service_init(&s_svc, &s_mock_input, 0.05f));
    TEST_ASSERT_FALSE(operator_service_is_brake_requested(&s_svc));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, operator_service_get_axis(&s_svc, 0));
    TEST_ASSERT_FALSE(operator_service_init(NULL, &s_mock_input, 0.05f));
    TEST_ASSERT_FALSE(operator_service_init(&s_svc, NULL, 0.05f));
}

void test_operator_service_deadband_filtering(void) {
    operator_service_init(&s_svc, &s_mock_input, 0.05f);

    // Giá trị trong vùng chết (<= 0.05f) phải lọc về 0.0f
    s_mock_axes[0] = 0.03f;
    s_mock_axes[1] = -0.04f;
    operator_service_update(&s_svc);

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, operator_service_get_axis(&s_svc, 0));
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, operator_service_get_axis(&s_svc, 1));
    TEST_ASSERT_FALSE(operator_service_has_motion_demand(&s_svc));

    // Vượt ngưỡng deadband
    s_mock_axes[0] = 0.5f;
    operator_service_update(&s_svc);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.4737f, operator_service_get_axis(&s_svc, 0));
    TEST_ASSERT_TRUE(operator_service_has_motion_demand(&s_svc));
}

void test_operator_service_brake_button_detection(void) {
    operator_service_init(&s_svc, &s_mock_input, 0.05f);

    s_mock_buttons[OPERATOR_BTN_BRAKE] = true;
    operator_service_update(&s_svc);
    TEST_ASSERT_TRUE(operator_service_is_brake_requested(&s_svc));

    s_mock_buttons[OPERATOR_BTN_BRAKE] = false;
    operator_service_update(&s_svc);
    TEST_ASSERT_FALSE(operator_service_is_brake_requested(&s_svc));
}


