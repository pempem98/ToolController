#include "unity.h"
#include "motor_service.h"
#include <string.h>

static bool s_driver_enabled = false;
static motor_direction_t s_driver_dir = MOTOR_DIR_CW;
static int32_t s_driver_pos = 0;
static int32_t s_driver_encoder = 12345;

static status_t mock_init(motor_interface_t *self) { (void)self; return STATUS_OK; }
static status_t mock_set_enabled(motor_interface_t *self, bool en) { (void)self; s_driver_enabled = en; return STATUS_OK; }
static status_t mock_set_direction(motor_interface_t *self, motor_direction_t dir) { (void)self; s_driver_dir = dir; return STATUS_OK; }
static status_t mock_move_to(motor_interface_t *self, int32_t pos, uint32_t spd, motor_direction_t dir) {
    (void)self; (void)spd; s_driver_pos = pos; s_driver_dir = dir; return STATUS_OK;
}
static status_t mock_rotate(motor_interface_t *self, motor_direction_t dir, uint32_t spd) {
    (void)self; (void)spd; s_driver_dir = dir; return STATUS_OK;
}
static status_t mock_get_pos(motor_interface_t *self, int32_t *pos) { (void)self; if (pos) *pos = s_driver_pos; return STATUS_OK; }
static status_t mock_get_encoder(motor_interface_t *self, int32_t *ticks) { (void)self; if (ticks) *ticks = s_driver_encoder; return STATUS_OK; }
static status_t mock_home(motor_interface_t *self) { (void)self; s_driver_pos = 0; return STATUS_OK; }
static status_t mock_stop(motor_interface_t *self) { (void)self; return STATUS_OK; }

static motor_interface_t s_mock_driver;
static motor_instance_t s_motors[4];
static motor_service_t s_svc;

void test_motor_service_setUp(void) {
    memset(&s_mock_driver, 0, sizeof(s_mock_driver));
    s_mock_driver.init = mock_init;
    s_mock_driver.set_enabled = mock_set_enabled;
    s_mock_driver.set_direction = mock_set_direction;
    s_mock_driver.move_to = mock_move_to;
    s_mock_driver.rotate = mock_rotate;
    s_mock_driver.get_actual_position = mock_get_pos;
    s_mock_driver.get_encoder_ticks = mock_get_encoder;
    s_mock_driver.home = mock_home;
    s_mock_driver.stop = mock_stop;

    s_driver_enabled = false;
    s_driver_dir = MOTOR_DIR_CW;
    s_driver_pos = 0;
    s_driver_encoder = 12345;

    memset(s_motors, 0, sizeof(s_motors));
    memset(&s_svc, 0, sizeof(s_svc));
}

void test_motor_service_tearDown(void) {
}

void test_motor_service_initialization(void) {
    TEST_ASSERT_TRUE(motor_service_init(&s_svc, s_motors, 4));
    TEST_ASSERT_EQUAL_UINT8(4, s_svc.motor_count);

    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_EQUAL_UINT8(i + 1, s_svc.motors[i].node_id);
        TEST_ASSERT_FALSE(s_svc.motors[i].enabled);
        TEST_ASSERT_NULL(s_svc.motors[i].driver);
    }
}

void test_motor_service_init_null_guards(void) {
    TEST_ASSERT_FALSE(motor_service_init(NULL, s_motors, 4));
    TEST_ASSERT_FALSE(motor_service_init(&s_svc, NULL, 4));
    TEST_ASSERT_FALSE(motor_service_init(&s_svc, s_motors, 0));
}

void test_motor_service_bind_driver(void) {
    motor_service_init(&s_svc, s_motors, 4);

    TEST_ASSERT_TRUE(motor_service_bind_driver(&s_svc, 0, &s_mock_driver));
    TEST_ASSERT_EQUAL_PTR(&s_mock_driver, s_svc.motors[0].driver);

    TEST_ASSERT_FALSE(motor_service_bind_driver(&s_svc, 4, &s_mock_driver));
    TEST_ASSERT_FALSE(motor_service_bind_driver(&s_svc, 5, &s_mock_driver));
    TEST_ASSERT_FALSE(motor_service_bind_driver(&s_svc, 0, NULL));
}

void test_motor_service_enable_disable_single_and_all(void) {
    motor_service_init(&s_svc, s_motors, 4);
    motor_service_bind_driver(&s_svc, 0, &s_mock_driver);

    TEST_ASSERT_TRUE(motor_service_enable_motor(&s_svc, 0));
    TEST_ASSERT_TRUE(s_svc.motors[0].enabled);
    TEST_ASSERT_TRUE(s_driver_enabled);

    TEST_ASSERT_TRUE(motor_service_disable_motor(&s_svc, 0));
    TEST_ASSERT_FALSE(s_svc.motors[0].enabled);
    TEST_ASSERT_FALSE(s_driver_enabled);

    TEST_ASSERT_TRUE(motor_service_enable_all(&s_svc));
    TEST_ASSERT_TRUE(s_svc.motors[0].enabled);

    TEST_ASSERT_TRUE(motor_service_disable_all(&s_svc));
    TEST_ASSERT_FALSE(s_svc.motors[0].enabled);
}

void test_motor_service_move_to_with_direction(void) {
    motor_service_init(&s_svc, s_motors, 4);
    motor_service_bind_driver(&s_svc, 1, &s_mock_driver);

    TEST_ASSERT_TRUE(motor_service_move_to(&s_svc, 1, 1000, 500, MOTOR_DIR_CCW));
    TEST_ASSERT_EQUAL_INT32(1000, s_driver_pos);
    TEST_ASSERT_EQUAL(MOTOR_DIR_CCW, s_driver_dir);
}

void test_motor_service_rotate_with_direction(void) {
    motor_service_init(&s_svc, s_motors, 4);
    motor_service_bind_driver(&s_svc, 1, &s_mock_driver);

    TEST_ASSERT_TRUE(motor_service_rotate(&s_svc, 1, MOTOR_DIR_CW, 2500));
    TEST_ASSERT_EQUAL(MOTOR_DIR_CW, s_driver_dir);
}

void test_motor_service_get_actual_position_and_encoder(void) {
    motor_service_init(&s_svc, s_motors, 4);
    motor_service_bind_driver(&s_svc, 1, &s_mock_driver);

    int32_t pos = 0, enc = 0;
    TEST_ASSERT_TRUE(motor_service_get_actual_position(&s_svc, 1, &pos));
    TEST_ASSERT_EQUAL_INT32(0, pos);

    TEST_ASSERT_TRUE(motor_service_get_encoder(&s_svc, 1, &enc));
    TEST_ASSERT_EQUAL_INT32(12345, enc);
}

void test_motor_service_home_and_stop(void) {
    motor_service_init(&s_svc, s_motors, 4);
    motor_service_bind_driver(&s_svc, 1, &s_mock_driver);

    s_driver_pos = 9999;
    TEST_ASSERT_TRUE(motor_service_home(&s_svc, 1));
    TEST_ASSERT_EQUAL_INT32(0, s_driver_pos);

    TEST_ASSERT_TRUE(motor_service_stop(&s_svc, 1));
    TEST_ASSERT_TRUE(motor_service_stop_all(&s_svc));
}

/* ---------------------------------------------------------------------
 * Additional coverage: NULL-function fallbacks, propagation of driver
 * failures across *_all helpers, and update() skip paths.
 * ------------------------------------------------------------------- */

/* A second mock driver with every function pointer left NULL, used to
 * exercise the fallback branches (driver present but specific fn NULL). */
static motor_interface_t s_null_fn_driver;

/* An error-returning driver used to test failure propagation in the
 * *_all() aggregate helpers. */
static status_t mock_rotate_fail(motor_interface_t *self, motor_direction_t dir, uint32_t spd) {
    (void)self; (void)dir; (void)spd; return STATUS_ERROR;
}
static status_t mock_move_to_fail(motor_interface_t *self, int32_t pos, uint32_t spd, motor_direction_t dir) {
    (void)self; (void)pos; (void)spd; (void)dir; return STATUS_ERROR;
}
static status_t mock_stop_fail(motor_interface_t *self) {
    (void)self; return STATUS_ERROR;
}
static motor_interface_t s_fail_driver;

void test_motor_service_set_direction_with_null_driver_function(void) {
    motor_service_init(&s_svc, s_motors, 4);
    memset(&s_null_fn_driver, 0, sizeof(s_null_fn_driver));
    motor_service_bind_driver(&s_svc, 0, &s_null_fn_driver);

    TEST_ASSERT_TRUE(motor_service_set_direction(&s_svc, 0, MOTOR_DIR_CCW));
    TEST_ASSERT_EQUAL(MOTOR_DIR_CCW, s_svc.motors[0].direction);
}

static status_t mock_set_direction_fail(motor_interface_t *self, motor_direction_t dir) {
    (void)self; (void)dir; return STATUS_ERROR;
}

void test_motor_service_set_direction_propagates_driver_failure(void) {
    motor_service_init(&s_svc, s_motors, 4);
    memset(&s_fail_driver, 0, sizeof(s_fail_driver));
    s_fail_driver.set_direction = mock_set_direction_fail;
    motor_service_bind_driver(&s_svc, 0, &s_fail_driver);

    TEST_ASSERT_FALSE(motor_service_set_direction(&s_svc, 0, MOTOR_DIR_CCW));
    /* direction trong struct van duoc cap nhat truoc khi goi driver (theo code), */
    /* chi gia tri tra ve phan anh loi tu driver. */
    TEST_ASSERT_EQUAL(MOTOR_DIR_CCW, s_svc.motors[0].direction);
}

void test_motor_service_get_position_fallback_when_driver_null(void) {
    motor_service_init(&s_svc, s_motors, 4);
    s_svc.motors[0].actual_position = 4242;

    int32_t pos = 0;
    TEST_ASSERT_TRUE(motor_service_get_actual_position(&s_svc, 0, &pos));
    TEST_ASSERT_EQUAL_INT32(4242, pos);
}

void test_motor_service_get_position_fallback_when_getter_null(void) {
    motor_service_init(&s_svc, s_motors, 4);
    memset(&s_null_fn_driver, 0, sizeof(s_null_fn_driver));
    motor_service_bind_driver(&s_svc, 0, &s_null_fn_driver);
    s_svc.motors[0].actual_position = 777;

    int32_t pos = 0;
    TEST_ASSERT_TRUE(motor_service_get_actual_position(&s_svc, 0, &pos));
    TEST_ASSERT_EQUAL_INT32(777, pos);
}

void test_motor_service_get_encoder_fallback_when_driver_null_and_getter_null(void) {
    motor_service_init(&s_svc, s_motors, 4);
    s_svc.motors[0].encoder_ticks = 555;

    int32_t ticks = 0;
    TEST_ASSERT_TRUE(motor_service_get_encoder(&s_svc, 0, &ticks));
    TEST_ASSERT_EQUAL_INT32(555, ticks);

    memset(&s_null_fn_driver, 0, sizeof(s_null_fn_driver));
    motor_service_bind_driver(&s_svc, 1, &s_null_fn_driver);
    s_svc.motors[1].encoder_ticks = 888;

    ticks = 0;
    TEST_ASSERT_TRUE(motor_service_get_encoder(&s_svc, 1, &ticks));
    TEST_ASSERT_EQUAL_INT32(888, ticks);
}

void test_motor_service_home_with_null_driver_and_null_home_fn(void) {
    motor_service_init(&s_svc, s_motors, 4);

    /* driver == NULL */
    TEST_ASSERT_TRUE(motor_service_home(&s_svc, 0));

    /* driver present but home() is NULL */
    memset(&s_null_fn_driver, 0, sizeof(s_null_fn_driver));
    motor_service_bind_driver(&s_svc, 1, &s_null_fn_driver);
    TEST_ASSERT_TRUE(motor_service_home(&s_svc, 1));
}

void test_motor_service_rotate_all_null_guards(void) {
    motor_service_init(&s_svc, s_motors, 4);
    motor_direction_t dirs[4] = { MOTOR_DIR_CW, MOTOR_DIR_CW, MOTOR_DIR_CW, MOTOR_DIR_CW };
    uint32_t speeds[4] = { 100, 100, 100, 100 };

    TEST_ASSERT_FALSE(motor_service_rotate_all(&s_svc, NULL, speeds));
    TEST_ASSERT_FALSE(motor_service_rotate_all(&s_svc, dirs, NULL));
    TEST_ASSERT_FALSE(motor_service_rotate_all(NULL, dirs, speeds));
}

void test_motor_service_move_all_null_guards(void) {
    motor_service_init(&s_svc, s_motors, 4);
    int32_t positions[4] = { 1, 2, 3, 4 };
    uint32_t speeds[4] = { 100, 100, 100, 100 };
    motor_direction_t dirs[4] = { MOTOR_DIR_CW, MOTOR_DIR_CW, MOTOR_DIR_CW, MOTOR_DIR_CW };

    TEST_ASSERT_FALSE(motor_service_move_all(&s_svc, NULL, speeds, dirs));
    TEST_ASSERT_FALSE(motor_service_move_all(&s_svc, positions, NULL, dirs));
    TEST_ASSERT_FALSE(motor_service_move_all(&s_svc, positions, speeds, NULL));
    TEST_ASSERT_FALSE(motor_service_move_all(NULL, positions, speeds, dirs));
}

void test_motor_service_rotate_all_propagates_failure(void) {
    motor_service_init(&s_svc, s_motors, 4);
    motor_service_bind_driver(&s_svc, 0, &s_mock_driver);

    memset(&s_fail_driver, 0, sizeof(s_fail_driver));
    s_fail_driver.rotate = mock_rotate_fail;
    motor_service_bind_driver(&s_svc, 1, &s_fail_driver);

    motor_direction_t dirs[4] = { MOTOR_DIR_CW, MOTOR_DIR_CW, MOTOR_DIR_CW, MOTOR_DIR_CW };
    uint32_t speeds[4] = { 100, 100, 100, 100 };

    TEST_ASSERT_FALSE(motor_service_rotate_all(&s_svc, dirs, speeds));
}

void test_motor_service_move_all_propagates_failure(void) {
    motor_service_init(&s_svc, s_motors, 4);
    motor_service_bind_driver(&s_svc, 0, &s_mock_driver);

    memset(&s_fail_driver, 0, sizeof(s_fail_driver));
    s_fail_driver.move_to = mock_move_to_fail;
    motor_service_bind_driver(&s_svc, 1, &s_fail_driver);

    int32_t positions[4] = { 10, 20, 30, 40 };
    uint32_t speeds[4] = { 100, 100, 100, 100 };
    motor_direction_t dirs[4] = { MOTOR_DIR_CW, MOTOR_DIR_CW, MOTOR_DIR_CW, MOTOR_DIR_CW };

    TEST_ASSERT_FALSE(motor_service_move_all(&s_svc, positions, speeds, dirs));
}

void test_motor_service_stop_all_propagates_failure(void) {
    motor_service_init(&s_svc, s_motors, 4);
    motor_service_bind_driver(&s_svc, 0, &s_mock_driver);

    memset(&s_fail_driver, 0, sizeof(s_fail_driver));
    s_fail_driver.stop = mock_stop_fail;
    motor_service_bind_driver(&s_svc, 1, &s_fail_driver);

    TEST_ASSERT_FALSE(motor_service_stop_all(&s_svc));
}

void test_motor_service_update_skips_null_driver(void) {
    motor_service_init(&s_svc, s_motors, 4);
    s_svc.motors[0].actual_position = 111;
    s_svc.motors[0].encoder_ticks = 222;

    TEST_ASSERT_TRUE(motor_service_update(&s_svc));

    TEST_ASSERT_EQUAL_INT32(111, s_svc.motors[0].actual_position);
    TEST_ASSERT_EQUAL_INT32(222, s_svc.motors[0].encoder_ticks);
}

void test_motor_service_update_skips_null_getter_functions(void) {
    motor_service_init(&s_svc, s_motors, 4);
    memset(&s_null_fn_driver, 0, sizeof(s_null_fn_driver));
    motor_service_bind_driver(&s_svc, 0, &s_null_fn_driver);

    s_svc.motors[0].actual_position = 333;
    s_svc.motors[0].encoder_ticks = 444;

    TEST_ASSERT_TRUE(motor_service_update(&s_svc));

    TEST_ASSERT_EQUAL_INT32(333, s_svc.motors[0].actual_position);
    TEST_ASSERT_EQUAL_INT32(444, s_svc.motors[0].encoder_ticks);
}
