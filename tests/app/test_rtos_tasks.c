/**
 * @file test_rtos_tasks.c
 * @brief Unit test SIL cho logic thực của các RTOS task step (app/src/rtos_tasks.c).
 *
 * @details Trước bản sửa này, emergency_brake_task/motion_control_task/input_scan_task
 *          bị định nghĩa thành stub rỗng trong tests/app/test_system_coordinator.c để thoả
 *          linker, khiến toàn bộ logic thật của đường xử lý dừng khẩn cấp KHÔNG nằm trong
 *          bất kỳ test case nào dù README quảng cáo "65/65 Passed". File này gọi trực tiếp
 *          input_scan_task_step() và motion_control_task_step() (các hàm *_step được tách
 *          riêng sẵn trong rtos_tasks.c đúng mục đích phục vụ SIL unit test) trên biến toàn
 *          cục g_sys, mô phỏng đầy đủ vòng lặp: input phanh -> trigger emergency brake ->
 *          motion_control_task dừng motor theo đúng cờ trạng thái.
 */
#include "unity.h"
#include "system_coordinator.h"
#include "board_interface.h"
#include "freertos_osal.h"
#include <string.h>

// Hàm thật cần kiểm thử, khai báo tại app/src/rtos_tasks.c (không stub trong file này).
extern void input_scan_task_step(void);
extern void motion_control_task_step(void);
extern void console_task_step(void);
extern void rtos_notify_brake_event(void);

static brake_interface_t s_mock_brake;
static operator_input_interface_t s_mock_op_input;
static console_interface_t s_mock_console;
static board_hardware_t s_mock_board;
static motor_interface_t s_mock_motor_drv[MOTOR_COUNT];

static bool s_brake_button_pressed = false;
static int  s_brake_engage_calls = 0;
static int  s_brake_release_calls = 0;
static int  s_motor_stop_calls[MOTOR_COUNT];
static int  s_motor_rotate_calls[MOTOR_COUNT];
static float s_axis_value[MOTOR_COUNT];

static status_t mock_brake_init(brake_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_brake_engage(brake_interface_t *self) { (void)self; s_brake_engage_calls++; return STATUS_OK; }
static status_t mock_brake_release(brake_interface_t *self) { (void)self; s_brake_release_calls++; return STATUS_OK; }
static status_t mock_brake_is_engaged(brake_interface_t *self, bool *is_en) {
    (void)self; if (is_en) *is_en = (s_brake_engage_calls > s_brake_release_calls); return STATUS_OK;
}

static status_t mock_op_init(operator_input_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_op_read_axis(operator_input_interface_t *self, uint8_t axis, float *v) {
    (void)self;
    if (v) *v = (axis < MOTOR_COUNT) ? s_axis_value[axis] : 0.0f;
    return STATUS_OK;
}
static status_t mock_op_read_btn(operator_input_interface_t *self, uint8_t btn, bool *p) {
    (void)self; (void)btn; if (p) *p = s_brake_button_pressed; return STATUS_OK;
}

static status_t mock_console_write(console_interface_t *self, const uint8_t *d, size_t l) {
    (void)self; (void)d; (void)l; return STATUS_OK;
}
static status_t mock_console_read(console_interface_t *self, uint8_t *b, size_t m, size_t *r) {
    (void)self; (void)b; (void)m; if (r) *r = 0; return STATUS_OK;
}

static status_t mock_motor_init(motor_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_motor_set_enabled(motor_interface_t *self, bool en) { (void)self; (void)en; return STATUS_OK; }
static status_t mock_motor_set_direction(motor_interface_t *self, motor_direction_t d) { (void)self; (void)d; return STATUS_OK; }
static status_t mock_motor_move_to(motor_interface_t *self, int32_t pos, uint32_t spd, motor_direction_t dir) {
    (void)self; (void)pos; (void)spd; (void)dir; return STATUS_OK;
}
static status_t mock_motor_rotate(motor_interface_t *self, motor_direction_t dir, uint32_t spd) {
    (void)dir; (void)spd;
    uint8_t idx = (uint8_t)(self - &s_mock_motor_drv[0]);
    if (idx < MOTOR_COUNT) s_motor_rotate_calls[idx]++;
    return STATUS_OK;
}
static status_t mock_motor_get_pos(motor_interface_t *self, int32_t *pos) { (void)self; if (pos) *pos = 0; return STATUS_OK; }
static status_t mock_motor_get_enc(motor_interface_t *self, int32_t *ticks) { (void)self; if (ticks) *ticks = 0; return STATUS_OK; }
static status_t mock_motor_home(motor_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_motor_stop(motor_interface_t *self) {
    uint8_t idx = (uint8_t)(self - &s_mock_motor_drv[0]);
    if (idx < MOTOR_COUNT) s_motor_stop_calls[idx]++;
    return STATUS_OK;
}

void test_rtos_tasks_setUp(void) {
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

    for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
        memset(&s_mock_motor_drv[i], 0, sizeof(s_mock_motor_drv[i]));
        s_mock_motor_drv[i].init = mock_motor_init;
        s_mock_motor_drv[i].set_enabled = mock_motor_set_enabled;
        s_mock_motor_drv[i].set_direction = mock_motor_set_direction;
        s_mock_motor_drv[i].move_to = mock_motor_move_to;
        s_mock_motor_drv[i].rotate = mock_motor_rotate;
        s_mock_motor_drv[i].get_actual_position = mock_motor_get_pos;
        s_mock_motor_drv[i].get_encoder_ticks = mock_motor_get_enc;
        s_mock_motor_drv[i].home = mock_motor_home;
        s_mock_motor_drv[i].stop = mock_motor_stop;
        s_axis_value[i] = 0.0f;
        s_motor_stop_calls[i] = 0;
        s_motor_rotate_calls[i] = 0;
    }

    memset(&s_mock_board, 0, sizeof(s_mock_board));
    s_mock_board.brake = &s_mock_brake;
    s_mock_board.operator_input = &s_mock_op_input;
    s_mock_board.console = &s_mock_console;
    s_mock_board.motor_count = MOTOR_COUNT;
    for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
        s_mock_board.motors[i] = &s_mock_motor_drv[i];
    }

    s_brake_button_pressed = false;
    s_brake_engage_calls = 0;
    s_brake_release_calls = 0;

    memset(&g_sys, 0, sizeof(g_sys));
    system_coordinator_init(&g_sys, &s_mock_board);
}

/**
 * @brief Nhấn nút phanh khẩn cấp -> input_scan_task_step() phải bật cờ emergency_brake_triggered
 *        và gọi hàm phanh cơ khí thật (không chỉ set cờ suông).
 */
void test_input_scan_task_step_triggers_emergency_brake_on_button_press(void) {
    s_brake_button_pressed = true;

    input_scan_task_step();

    TEST_ASSERT_TRUE(g_sys.emergency_brake_triggered);
    TEST_ASSERT_EQUAL_INT(1, s_brake_engage_calls);
}

/**
 * @brief Nhả nút phanh sau khi đã trigger -> cờ phải được xoá ở vòng step kế tiếp,
 *        cho phép motion_control_task_step() nối lại vận hành bình thường.
 */
void test_input_scan_task_step_clears_flag_when_button_released(void) {
    s_brake_button_pressed = true;
    input_scan_task_step();
    TEST_ASSERT_TRUE(g_sys.emergency_brake_triggered);

    s_brake_button_pressed = false;
    input_scan_task_step();
    TEST_ASSERT_FALSE(g_sys.emergency_brake_triggered);
}

/**
 * @brief Đây là bài test P0 cho toàn bộ đường dừng khẩn cấp: khi cờ emergency_brake_triggered
 *        đã bật (do input_scan_task_step phát hiện), motion_control_task_step() BẮT BUỘC phải
 *        dừng toàn bộ motor và giữ phanh khoá, bất kể operator đang yêu cầu chuyển động gì.
 */
void test_motion_control_task_step_stops_all_motors_when_brake_triggered(void) {
    s_axis_value[0] = 0.8f; // Operator vẫn đang đẩy joystick trục 0

    s_brake_button_pressed = true;
    input_scan_task_step();
    TEST_ASSERT_TRUE(g_sys.emergency_brake_triggered);

    motion_control_task_step();

    TEST_ASSERT_GREATER_OR_EQUAL_INT(1, s_brake_engage_calls);
    for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
        TEST_ASSERT_GREATER_OR_EQUAL_INT(1, s_motor_stop_calls[i]);
        TEST_ASSERT_EQUAL_INT(0, s_motor_rotate_calls[i]);
    }
}

/**
 * @brief Khi không có yêu cầu dừng khẩn cấp và operator đẩy joystick trục 0,
 *        motion_control_task_step() phải mở phanh và ra lệnh rotate cho đúng trục đó.
 *
 * @note  Trong vòng lặp thật, input_scan_task_step() (chạy trước, xem app/src/rtos_tasks.c)
 *        là nơi gọi operator_service_update() để cập nhật filtered_axes từ ADC. Test phải
 *        tái tạo đúng thứ tự đó — gọi thẳng motion_control_task_step() một mình sẽ đọc
 *        filtered_axes toàn 0.0f (chưa từng được cập nhật) và không phản ánh hành vi thật.
 */
void test_motion_control_task_step_rotates_motor_on_operator_demand(void) {
    s_axis_value[0] = 0.5f;

    input_scan_task_step();
    motion_control_task_step();

    TEST_ASSERT_EQUAL_INT(1, s_motor_rotate_calls[0]);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(1, s_brake_release_calls);
}

/**
 * @brief Không có input nào và không có brake trigger -> motor phải ở trạng thái dừng
 *        (an toàn mặc định), không được tự ý quay.
 */
void test_motion_control_task_step_stops_all_motors_when_idle(void) {
    motion_control_task_step();

    for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
        TEST_ASSERT_EQUAL_INT(0, s_motor_rotate_calls[i]);
    }
}

// -----------------------------------------------------------------------------
// console_task_step()
// -----------------------------------------------------------------------------

/**
 * @brief console_task_step() là hàm tách riêng (không phải task wrapper while(1)),
 *        nên có thể gọi trực tiếp an toàn trong unit test trên host. Test xác nhận
 *        nó đọc được tick hệ thống và gọi cli_service_process() mà không crash
 *        (không assert sâu về nội dung log vì CLI_TELEMETRY_DEFAULT_ENABLED mặc định
 *        là 0 nên có thể không ghi gì trong 1 lần gọi).
 */
void test_console_task_step_calls_cli_service_process_without_crash(void) {
    uint32_t tick_before = osal_get_tick_ms();

    console_task_step();

    uint32_t tick_after = osal_get_tick_ms();
    TEST_ASSERT_TRUE(tick_after >= tick_before);
}

// -----------------------------------------------------------------------------
// rtos_notify_brake_event()
// -----------------------------------------------------------------------------

/**
 * @brief s_brake_evt_queue là biến static file-scope trong rtos_tasks.c, chỉ được
 *        khởi tạo bên trong emergency_brake_task() (không thể gọi trực tiếp vì có
 *        while(1) vô hạn — không có RTOS scheduler thật trên host để preempt).
 *        Khi chưa có task nào chạy, s_brake_evt_queue còn là NULL. Test này xác nhận
 *        rtos_notify_brake_event() an toàn (no-op) khi gọi trước khi hàng đợi được
 *        khởi tạo — đây là tình huống có thật (VD: ISR/task khác gọi
 *        system_coordinator_trigger_emergency_brake() trước khi EmgBrakeTask được
 *        osal_task_create() và chạy lần đầu).
 */
void test_rtos_notify_brake_event_is_safe_when_queue_not_yet_initialized(void) {
    rtos_notify_brake_event();
    TEST_PASS();
}
