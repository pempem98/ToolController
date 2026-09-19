#include "unity.h"
#include "system_coordinator.h"
#include "board_interface.h"
#include <string.h>

void emergency_brake_task(void *pvParameters) { (void)pvParameters; }
void motion_control_task(void *pvParameters) { (void)pvParameters; }
void input_scan_task(void *pvParameters) { (void)pvParameters; }
void console_task(void *pvParameters) { (void)pvParameters; }
void rtos_notify_brake_event(void) {}
status_t board_init(void) { return STATUS_OK; }
const board_hardware_t* board_get_hardware(void) { return NULL; }

static brake_interface_t s_mock_brake;
static operator_input_interface_t s_mock_op_input;
static console_interface_t s_mock_console;
static board_hardware_t s_mock_board;
static system_coordinator_t s_sys;

static status_t mock_brake_init(brake_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_brake_engage(brake_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_brake_release(brake_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_brake_is_engaged(brake_interface_t *self, bool *is_en) {
    (void)self; if (is_en) *is_en = true; return STATUS_OK;
}

static status_t mock_op_init(operator_input_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_op_read_axis(operator_input_interface_t *self, uint8_t axis, float *v) {
    (void)self; (void)axis; if (v) *v = 0.0f; return STATUS_OK;
}
static status_t mock_op_read_btn(operator_input_interface_t *self, uint8_t btn, bool *p) {
    (void)self; (void)btn; if (p) *p = false; return STATUS_OK;
}

static status_t mock_console_write(console_interface_t *self, const uint8_t *d, size_t l) {
    (void)self; (void)d; (void)l; return STATUS_OK;
}
static status_t mock_console_read(console_interface_t *self, uint8_t *b, size_t m, size_t *r) {
    (void)self; (void)b; (void)m; if (r) *r = 0; return STATUS_OK;
}

void test_system_coordinator_setUp(void) {
    memset(&s_mock_brake, 0, sizeof(s_mock_brake));
    s_mock_brake.init = mock_brake_init;
    s_mock_brake.engage = mock_brake_engage;
    s_mock_brake.release = mock_brake_release;
    s_mock_brake.is_engaged = mock_brake_is_engaged;

    memset(&s_mock_op_input, 0, sizeof(s_mock_op_input));
    s_mock_op_input.init = mock_op_init;
    s_mock_op_input.read_axis = mock_op_read_axis;
    s_mock_op_input.read_button = mock_op_read_btn;

    memset(&s_mock_console, 0, sizeof(s_mock_console));
    s_mock_console.write = mock_console_write;
    s_mock_console.read = mock_console_read;

    memset(&s_mock_board, 0, sizeof(s_mock_board));
    s_mock_board.brake = &s_mock_brake;
    s_mock_board.operator_input = &s_mock_op_input;
    s_mock_board.console = &s_mock_console;
    s_mock_board.motor_count = 0;

    memset(&s_sys, 0, sizeof(s_sys));
}

void test_system_coordinator_tearDown(void) {
}

void test_system_coordinator_init_null_sys_returns_false(void) {
    TEST_ASSERT_FALSE(system_coordinator_init(NULL, &s_mock_board));
    TEST_ASSERT_FALSE(system_coordinator_init(&s_sys, NULL));
}

void test_system_coordinator_init_success(void) {
    TEST_ASSERT_TRUE(system_coordinator_init(&s_sys, &s_mock_board));
    TEST_ASSERT_FALSE(s_sys.emergency_brake_triggered);
}

void test_system_coordinator_trigger_emergency_brake(void) {
    system_coordinator_init(&s_sys, &s_mock_board);

    system_coordinator_trigger_emergency_brake(&s_sys);
    TEST_ASSERT_TRUE(s_sys.emergency_brake_triggered);
}


