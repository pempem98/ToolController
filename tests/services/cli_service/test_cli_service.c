/**
 * @file test_cli_service.c
 * @brief Unit test cho cli_service.c: khoi tao, bat/tat telemetry, xu ly lenh CLI
 *        (help/status/stop_all/brake/log/rotate/move/rotate_all/move_all/unknown),
 *        backspace, tran buffer va telemetry dinh ky.
 *
 * Console duoc mock bang mot buffer tinh de ghi lai toan bo output cua write(),
 * va mot chuoi input dinh san duoc "nhai" tung byte mot qua read() de mo phong
 * nguoi dung go lenh qua UART/console that.
 */
#include "unity.h"
#include "cli_service.h"
#include <string.h>

/* ------------------------------------------------------------------------ */
/* Mock console_interface_t                                                  */
/* ------------------------------------------------------------------------ */

static char   s_console_out[4096];
static size_t s_console_out_len;

static const char *s_console_in;
static size_t       s_console_in_len;
static size_t       s_console_in_pos;

static status_t mock_console_write(console_interface_t *self, const uint8_t *data, size_t len) {
    (void)self;
    if (!data) return STATUS_INVALID_PARAM;
    if (s_console_out_len + len < sizeof(s_console_out)) {
        memcpy(s_console_out + s_console_out_len, data, len);
        s_console_out_len += len;
        s_console_out[s_console_out_len] = '\0';
    }
    return STATUS_OK;
}

/* Tra ve dung 1 byte moi lan goi, mo phong UART nhan tung byte. */
static status_t mock_console_read(console_interface_t *self, uint8_t *buffer, size_t max_len, size_t *received_len) {
    (void)self;
    if (!buffer || !received_len || max_len == 0) return STATUS_INVALID_PARAM;
    if (s_console_in_pos >= s_console_in_len) {
        *received_len = 0;
        return STATUS_OK;
    }
    buffer[0] = (uint8_t)s_console_in[s_console_in_pos++];
    *received_len = 1;
    return STATUS_OK;
}

static console_interface_t s_console;
static cli_service_t       s_svc;

/* Cac service phu thuoc that (khong mock service, chi mock driver hardware) */
static motor_service_t    s_motor_svc;
static motor_instance_t   s_motors[4];
static operator_service_t s_op_svc;
static brake_service_t    s_brake_svc;

static operator_input_interface_t s_op_input;
static brake_interface_t          s_brake_hw;

static status_t mock_op_init(operator_input_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_op_read_axis(operator_input_interface_t *self, uint8_t axis, float *val) {
    (void)self; (void)axis; if (val) *val = 0.0f; return STATUS_OK;
}
static status_t mock_op_read_button(operator_input_interface_t *self, uint8_t btn, bool *pressed) {
    (void)self; (void)btn; if (pressed) *pressed = false; return STATUS_OK;
}

static status_t mock_brake_init(brake_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_brake_engage(brake_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_brake_release(brake_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_brake_is_engaged(brake_interface_t *self, bool *is_en) { (void)self; if (is_en) *is_en = true; return STATUS_OK; }

void test_cli_service_setUp(void) {
    memset(s_console_out, 0, sizeof(s_console_out));
    s_console_out_len = 0;
    s_console_in = NULL;
    s_console_in_len = 0;
    s_console_in_pos = 0;

    memset(&s_console, 0, sizeof(s_console));
    s_console.write = mock_console_write;
    s_console.read = mock_console_read;

    memset(&s_motors, 0, sizeof(s_motors));
    memset(&s_motor_svc, 0, sizeof(s_motor_svc));
    motor_service_init(&s_motor_svc, s_motors, 4);

    memset(&s_op_input, 0, sizeof(s_op_input));
    s_op_input.init = mock_op_init;
    s_op_input.read_axis = mock_op_read_axis;
    s_op_input.read_button = mock_op_read_button;
    memset(&s_op_svc, 0, sizeof(s_op_svc));
    operator_service_init(&s_op_svc, &s_op_input, 0.05f);

    memset(&s_brake_hw, 0, sizeof(s_brake_hw));
    s_brake_hw.init = mock_brake_init;
    s_brake_hw.engage = mock_brake_engage;
    s_brake_hw.release = mock_brake_release;
    s_brake_hw.is_engaged = mock_brake_is_engaged;
    memset(&s_brake_svc, 0, sizeof(s_brake_svc));
    brake_service_init(&s_brake_svc, &s_brake_hw, 100);

    memset(&s_svc, 0, sizeof(s_svc));
}

void test_cli_service_tearDown(void) {
}

/* Helper: nap chuoi lenh test lam nguon input cho mock_console_read() */
static void feed_input(const char *cmd_line) {
    s_console_in = cmd_line;
    s_console_in_len = strlen(cmd_line);
    s_console_in_pos = 0;
}

/* ------------------------------------------------------------------------ */
/* 1. cli_service_init                                                       */
/* ------------------------------------------------------------------------ */

void test_cli_service_init_null_guards(void) {
    TEST_ASSERT_FALSE(cli_service_init(NULL, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc));
    TEST_ASSERT_FALSE(cli_service_init(&s_svc, NULL, &s_motor_svc, &s_op_svc, &s_brake_svc));
}

void test_cli_service_init_success_full_services(void) {
    TEST_ASSERT_TRUE(cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc));
    TEST_ASSERT_EQUAL_PTR(&s_console, s_svc.console);
    TEST_ASSERT_EQUAL_PTR(&s_motor_svc, s_svc.motor_svc);
    TEST_ASSERT_EQUAL_PTR(&s_op_svc, s_svc.op_svc);
    TEST_ASSERT_EQUAL_PTR(&s_brake_svc, s_svc.brake_svc);
    TEST_ASSERT_EQUAL_UINT8(0, s_svc.rx_index);
    TEST_ASSERT_EQUAL_UINT32(500, s_svc.telemetry_interval_ms);
    /* Init phai in duoc banner chao mung */
    TEST_ASSERT_TRUE(strstr(s_console_out, "Initialized") != NULL);
    TEST_ASSERT_TRUE(strstr(s_console_out, "help") != NULL);
}

void test_cli_service_init_success_optional_services_null(void) {
    TEST_ASSERT_TRUE(cli_service_init(&s_svc, &s_console, NULL, NULL, NULL));
    TEST_ASSERT_NULL(s_svc.motor_svc);
    TEST_ASSERT_NULL(s_svc.op_svc);
    TEST_ASSERT_NULL(s_svc.brake_svc);
}

/* ------------------------------------------------------------------------ */
/* 2. cli_service_set_telemetry_enabled                                      */
/* ------------------------------------------------------------------------ */

void test_cli_service_set_telemetry_enabled_toggle(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);

    cli_service_set_telemetry_enabled(&s_svc, true);
    TEST_ASSERT_TRUE(s_svc.telemetry_enabled);

    cli_service_set_telemetry_enabled(&s_svc, false);
    TEST_ASSERT_FALSE(s_svc.telemetry_enabled);
}

void test_cli_service_set_telemetry_enabled_null_guard(void) {
    /* Khong duoc crash khi svc la NULL */
    cli_service_set_telemetry_enabled(NULL, true);
    TEST_PASS();
}

/* ------------------------------------------------------------------------ */
/* 3. cli_service_process: cac lenh qua console mock                        */
/* ------------------------------------------------------------------------ */

void test_cli_process_help_command(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("help\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "Available Commands") != NULL);
}

void test_cli_process_status_command(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("status\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[STATUS]") != NULL);
}

void test_cli_process_stop_all_command(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("stop_all\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[OK]") != NULL);
    TEST_ASSERT_TRUE(strstr(s_console_out, "stopped") != NULL);
}

void test_cli_process_brake_on_off(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);

    s_console_out_len = 0; s_console_out[0] = '\0';
    feed_input("brake on\r\n");
    cli_service_process(&s_svc, 0);
    TEST_ASSERT_TRUE(strstr(s_console_out, "[OK]") != NULL);
    TEST_ASSERT_TRUE(strstr(s_console_out, "engaged") != NULL);

    s_console_out_len = 0; s_console_out[0] = '\0';
    feed_input("brake off\r\n");
    cli_service_process(&s_svc, 0);
    TEST_ASSERT_TRUE(strstr(s_console_out, "[OK]") != NULL);
    TEST_ASSERT_TRUE(strstr(s_console_out, "released") != NULL);
}

void test_cli_process_log_on_off(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);

    s_console_out_len = 0; s_console_out[0] = '\0';
    feed_input("log on\r\n");
    cli_service_process(&s_svc, 0);
    TEST_ASSERT_TRUE(s_svc.telemetry_enabled);
    TEST_ASSERT_TRUE(strstr(s_console_out, "Log ON") != NULL);

    s_console_out_len = 0; s_console_out[0] = '\0';
    feed_input("log off\r\n");
    cli_service_process(&s_svc, 0);
    TEST_ASSERT_FALSE(s_svc.telemetry_enabled);
    TEST_ASSERT_TRUE(strstr(s_console_out, "Log OFF") != NULL);
}

void test_cli_process_rotate_command(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("rotate 0 cw 500\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[OK] Rotating motor 0") != NULL);
}

void test_cli_process_move_command(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    /* dang "move <axis> <dir> <pos> <speed>" duoc parser ho tro */
    feed_input("move 0 cw 100 500\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[OK] Moving motor 0 to 100") != NULL);
}

void test_cli_process_rotate_all_command(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("rotate_all cw 100 ccw 200 cw 300 ccw 400\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[OK] Executed rotate_all") != NULL);
}

void test_cli_process_move_all_command(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("move_all 10 20 30 40 500\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[OK] Executed move_all") != NULL);
}

void test_cli_process_unknown_command(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("frobnicate\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[ERR]") != NULL);
}

/* ------------------------------------------------------------------------ */
/* 4. Backspace                                                              */
/* ------------------------------------------------------------------------ */

void test_cli_process_backspace_removes_last_char(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    /* Go "statusx", xoa 'x' bang backspace (0x08), roi Enter -> phai chay "status" */
    feed_input("statusx\x08\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_EQUAL_UINT8(0, s_svc.rx_index);
    TEST_ASSERT_TRUE(strstr(s_console_out, "[STATUS]") != NULL);
}

void test_cli_process_backspace_0x7f_variant(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("statusy\x7f\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[STATUS]") != NULL);
}

void test_cli_process_backspace_on_empty_buffer_noop(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    /* Backspace khi buffer rong khong duoc lam rx_index underflow (uint8_t) */
    feed_input("\x08\x08help\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "Available Commands") != NULL);
}

/* ------------------------------------------------------------------------ */
/* 5. Buffer day (khong duoc tran bo nho)                                    */
/* ------------------------------------------------------------------------ */

void test_cli_process_rx_buffer_overflow_guard(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    s_console_out_len = 0; s_console_out[0] = '\0';

    /* Go nhieu hon CLI_SERVICE_RX_BUFFER_SIZE ky tu 'a' ma khong Enter.
     * Code gac bang "svc->rx_index < CLI_SERVICE_RX_BUFFER_SIZE - 1" nen
     * rx_index khong duoc vuot qua CLI_SERVICE_RX_BUFFER_SIZE - 1. */
    char overflow_input[CLI_SERVICE_RX_BUFFER_SIZE * 2 + 1];
    memset(overflow_input, 'a', sizeof(overflow_input) - 1);
    overflow_input[sizeof(overflow_input) - 1] = '\0';

    feed_input(overflow_input);
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(s_svc.rx_index <= CLI_SERVICE_RX_BUFFER_SIZE - 1);
    /* rx_buffer phai van la chuoi hop le (duoc null-terminated tai vi tri cuoi) */
    TEST_ASSERT_EQUAL_UINT8('\0', (uint8_t)s_svc.rx_buffer[CLI_SERVICE_RX_BUFFER_SIZE - 1]);
}

/* ------------------------------------------------------------------------ */
/* 6. Telemetry dinh ky                                                      */
/* ------------------------------------------------------------------------ */

void test_cli_process_periodic_telemetry_emits_when_interval_elapsed(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    cli_service_set_telemetry_enabled(&s_svc, true);
    s_svc.last_telemetry_tick = 0;

    s_console_out_len = 0; s_console_out[0] = '\0';
    feed_input(""); /* khong co lenh nao, chi test nhanh telemetry */
    cli_service_process(&s_svc, 600); /* > telemetry_interval_ms (500) */

    TEST_ASSERT_TRUE(strstr(s_console_out, "TLM") != NULL);
    TEST_ASSERT_EQUAL_UINT32(600, s_svc.last_telemetry_tick);
}

void test_cli_process_periodic_telemetry_not_emitted_before_interval(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    cli_service_set_telemetry_enabled(&s_svc, true);
    s_svc.last_telemetry_tick = 0;

    /* Lan goi dau du de kich hoat telemetry */
    feed_input("");
    cli_service_process(&s_svc, 600);

    /* Lan goi thu hai ngay sau, chua du khoang thoi gian -> khong duoc in them TLM */
    s_console_out_len = 0; s_console_out[0] = '\0';
    feed_input("");
    cli_service_process(&s_svc, 700); /* 700 - 600 = 100 < 500 */

    TEST_ASSERT_TRUE(strstr(s_console_out, "TLM") == NULL);
}

void test_cli_process_telemetry_disabled_no_output(void) {
    cli_service_init(&s_svc, &s_console, &s_motor_svc, &s_op_svc, &s_brake_svc);
    cli_service_set_telemetry_enabled(&s_svc, false);
    s_svc.last_telemetry_tick = 0;

    s_console_out_len = 0; s_console_out[0] = '\0';
    feed_input("");
    cli_service_process(&s_svc, 10000);

    TEST_ASSERT_TRUE(strstr(s_console_out, "TLM") == NULL);
}

/* ------------------------------------------------------------------------ */
/* 7. Cac service phu (motor/op/brake) la NULL: khong crash, van phan hoi   */
/* ------------------------------------------------------------------------ */

void test_cli_process_status_with_all_services_null(void) {
    TEST_ASSERT_TRUE(cli_service_init(&s_svc, &s_console, NULL, NULL, NULL));
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("status\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[STATUS]") != NULL);
    TEST_ASSERT_TRUE(strstr(s_console_out, "RELEASED") != NULL); /* brake_svc NULL -> khong ENGAGED */
}

void test_cli_process_stop_all_with_motor_svc_null(void) {
    TEST_ASSERT_TRUE(cli_service_init(&s_svc, &s_console, NULL, NULL, NULL));
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("stop_all\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[OK]") != NULL);
}

void test_cli_process_brake_with_brake_svc_null(void) {
    TEST_ASSERT_TRUE(cli_service_init(&s_svc, &s_console, NULL, NULL, NULL));
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("brake on\r\n");
    cli_service_process(&s_svc, 0);

    TEST_ASSERT_TRUE(strstr(s_console_out, "[OK]") != NULL);
}

void test_cli_process_rotate_with_motor_svc_null(void) {
    TEST_ASSERT_TRUE(cli_service_init(&s_svc, &s_console, NULL, NULL, NULL));
    s_console_out_len = 0; s_console_out[0] = '\0';

    feed_input("rotate 0 cw 500\r\n");
    cli_service_process(&s_svc, 0);

    /* motor_svc NULL -> nhanh if(svc->motor_svc) khong chay, nhung van phai in prompt
     * ma khong crash. Khong co [OK] vi block bi bo qua hoan toan. */
    TEST_ASSERT_TRUE(strstr(s_console_out, "> ") != NULL);
}

void test_cli_process_telemetry_with_services_null(void) {
    TEST_ASSERT_TRUE(cli_service_init(&s_svc, &s_console, NULL, NULL, NULL));
    cli_service_set_telemetry_enabled(&s_svc, true);
    s_svc.last_telemetry_tick = 0;

    s_console_out_len = 0; s_console_out[0] = '\0';
    feed_input("");
    cli_service_process(&s_svc, 1000);

    TEST_ASSERT_TRUE(strstr(s_console_out, "TLM") != NULL);
}
