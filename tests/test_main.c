#include "unity.h"
#include <stdio.h>

// Suite setup functions
extern void test_bldc_can_driver_setUp(void);
extern void test_tmc2209_driver_setUp(void);
extern void test_brake_driver_setUp(void);
extern void test_operator_input_driver_setUp(void);
extern void test_operator_input_driver_tearDown(void);
extern void test_motor_service_setUp(void);
extern void test_brake_service_setUp(void);
extern void test_operator_service_setUp(void);
extern void test_cli_protocol_setUp(void);
extern void test_cli_service_setUp(void);
extern void test_system_coordinator_setUp(void);
extern void test_rtos_tasks_setUp(void);
extern void test_freertos_osal_setUp(void);

static void (*s_current_setup)(void) = NULL;
static void (*s_current_teardown)(void) = NULL;

void setUp(void) {
    if (s_current_setup) {
        s_current_setup();
    }
}

void tearDown(void) {
    if (s_current_teardown) {
        s_current_teardown();
    }
}

// Declarations of driver test runners
extern void test_bldc_driver_create_populates_interface(void);
extern void test_bldc_driver_init_resets_state(void);
extern void test_bldc_driver_set_enabled_true_packs_enable_command(void);
extern void test_bldc_driver_set_enabled_false_packs_disable_command(void);
extern void test_bldc_driver_set_direction_updates_priv(void);
extern void test_bldc_driver_move_to_packs_position_and_speed(void);
extern void test_bldc_driver_rotate_packs_speed_and_direction(void);
extern void test_bldc_driver_home_sends_zero_command(void);
extern void test_bldc_driver_stop_sends_emergency_stop(void);
extern void test_bldc_driver_get_position_and_ticks(void);
extern void test_bldc_driver_process_rx_updates_state(void);
extern void test_bldc_driver_process_rx_ignores_wrong_node_id(void);
extern void test_bldc_driver_null_guards(void);

extern void test_tmc2209_create_populates_interface(void);
extern void test_tmc2209_init_disables_motor_pin_high(void);
extern void test_tmc2209_enable_sets_pin_low(void);
extern void test_tmc2209_set_direction_normal_and_inverted(void);
extern void test_tmc2209_move_to_updates_position(void);
extern void test_tmc2209_home_resets_position(void);
extern void test_tmc2209_null_guards(void);

extern void test_brake_create_populates_interface(void);
extern void test_brake_init_engages_brake(void);
extern void test_brake_release_sets_pin_high(void);
extern void test_brake_engage_sets_pin_low(void);
extern void test_brake_null_guards(void);

extern void test_operator_create_populates_interface(void);
extern void test_operator_compute_axis_norm_values(void);
extern void test_operator_read_axis_uses_buffer(void);
extern void test_operator_read_button_brake_active_low(void);
extern void test_operator_null_guards(void);

// Declarations of service test runners
extern void test_motor_service_initialization(void);
extern void test_motor_service_init_null_guards(void);
extern void test_motor_service_bind_driver(void);
extern void test_motor_service_enable_disable_single_and_all(void);
extern void test_motor_service_move_to_with_direction(void);
extern void test_motor_service_rotate_with_direction(void);
extern void test_motor_service_get_actual_position_and_encoder(void);
extern void test_motor_service_home_and_stop(void);
extern void test_motor_service_set_direction_with_null_driver_function(void);
extern void test_motor_service_set_direction_propagates_driver_failure(void);
extern void test_motor_service_get_position_fallback_when_driver_null(void);
extern void test_motor_service_get_position_fallback_when_getter_null(void);
extern void test_motor_service_get_encoder_fallback_when_driver_null_and_getter_null(void);
extern void test_motor_service_home_with_null_driver_and_null_home_fn(void);
extern void test_motor_service_rotate_all_null_guards(void);
extern void test_motor_service_move_all_null_guards(void);
extern void test_motor_service_rotate_all_propagates_failure(void);
extern void test_motor_service_move_all_propagates_failure(void);
extern void test_motor_service_stop_all_propagates_failure(void);
extern void test_motor_service_update_skips_null_driver(void);
extern void test_motor_service_update_skips_null_getter_functions(void);

extern void test_brake_service_init_success(void);
extern void test_brake_service_init_null_guards(void);
extern void test_brake_service_release_success(void);
extern void test_brake_service_engage_success(void);
extern void test_brake_service_get_state(void);

extern void test_operator_service_init_success(void);
extern void test_operator_service_deadband_filtering(void);
extern void test_operator_service_brake_button_detection(void);

// Declarations of cli_service test runners
extern void test_cli_service_init_null_guards(void);
extern void test_cli_service_init_success_full_services(void);
extern void test_cli_service_init_success_optional_services_null(void);
extern void test_cli_service_set_telemetry_enabled_toggle(void);
extern void test_cli_service_set_telemetry_enabled_null_guard(void);
extern void test_cli_process_help_command(void);
extern void test_cli_process_status_command(void);
extern void test_cli_process_stop_all_command(void);
extern void test_cli_process_brake_on_off(void);
extern void test_cli_process_log_on_off(void);
extern void test_cli_process_rotate_command(void);
extern void test_cli_process_move_command(void);
extern void test_cli_process_rotate_all_command(void);
extern void test_cli_process_move_all_command(void);
extern void test_cli_process_unknown_command(void);
extern void test_cli_process_backspace_removes_last_char(void);
extern void test_cli_process_backspace_0x7f_variant(void);
extern void test_cli_process_backspace_on_empty_buffer_noop(void);
extern void test_cli_process_rx_buffer_overflow_guard(void);
extern void test_cli_process_periodic_telemetry_emits_when_interval_elapsed(void);
extern void test_cli_process_periodic_telemetry_not_emitted_before_interval(void);
extern void test_cli_process_telemetry_disabled_no_output(void);
extern void test_cli_process_status_with_all_services_null(void);
extern void test_cli_process_stop_all_with_motor_svc_null(void);
extern void test_cli_process_brake_with_brake_svc_null(void);
extern void test_cli_process_rotate_with_motor_svc_null(void);
extern void test_cli_process_telemetry_with_services_null(void);

// Declarations of app coordinator test runners
extern void test_system_coordinator_init_null_sys_returns_false(void);
extern void test_system_coordinator_init_success(void);
extern void test_system_coordinator_trigger_emergency_brake(void);
extern void test_system_coordinator_start_tasks_returns_true(void);

// Declarations of RTOS task step test runners (real emergency-stop path, not stubbed)
extern void test_input_scan_task_step_triggers_emergency_brake_on_button_press(void);
extern void test_input_scan_task_step_clears_flag_when_button_released(void);
extern void test_motion_control_task_step_stops_all_motors_when_brake_triggered(void);
extern void test_motion_control_task_step_rotates_motor_on_operator_demand(void);
extern void test_motion_control_task_step_stops_all_motors_when_idle(void);
extern void test_console_task_step_calls_cli_service_process_without_crash(void);
extern void test_rtos_notify_brake_event_is_safe_when_queue_not_yet_initialized(void);

// Declarations of connectivity test runners
extern void test_cli_parse_help(void);
extern void test_cli_parse_status(void);
extern void test_cli_parse_stop_all(void);
extern void test_cli_parse_brake_on_off(void);
extern void test_cli_parse_rotate_single_axis(void);
extern void test_cli_parse_move_single_axis(void);
extern void test_cli_parse_rotate_all(void);
extern void test_cli_parse_move_all(void);
extern void test_cli_parse_log_on_off(void);
extern void test_cli_format_telemetry(void);
extern void test_cli_parse_null_and_invalid(void);
extern void test_cli_parse_empty_and_whitespace_only(void);
extern void test_cli_parse_too_long_string(void);
extern void test_cli_parse_brake_invalid_arg(void);
extern void test_cli_parse_log_invalid_arg(void);
extern void test_cli_parse_rotate_all_numeric_fallback_format(void);
extern void test_cli_parse_rotate_all_insufficient_args_fails(void);
extern void test_cli_parse_move_all_insufficient_args_fails(void);
extern void test_cli_parse_move_numeric_direction_format(void);
extern void test_cli_parse_move_numeric_direction_insufficient_args_fails(void);
extern void test_cli_format_telemetry_buffer_too_small(void);
extern void test_cli_format_telemetry_null_buffer(void);

// Declarations of middleware osal test runners
extern void test_osal_task_create_and_delete(void);
extern void test_osal_mutex_lock_unlock(void);
extern void test_osal_queue_send_receive(void);
extern void test_osal_timer_start_stop(void);
extern void test_osal_time_utilities(void);

int main(void) {
    UNITY_BEGIN();

    printf("\n========================================\n");
    printf("  1. HARDWARE DRIVERS TESTS (STM32H7A3ZIT6Q)  \n");
    printf("========================================\n");
    s_current_setup = test_bldc_can_driver_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_bldc_driver_create_populates_interface);
    RUN_TEST(test_bldc_driver_init_resets_state);
    RUN_TEST(test_bldc_driver_set_enabled_true_packs_enable_command);
    RUN_TEST(test_bldc_driver_set_enabled_false_packs_disable_command);
    RUN_TEST(test_bldc_driver_set_direction_updates_priv);
    RUN_TEST(test_bldc_driver_move_to_packs_position_and_speed);
    RUN_TEST(test_bldc_driver_rotate_packs_speed_and_direction);
    RUN_TEST(test_bldc_driver_home_sends_zero_command);
    RUN_TEST(test_bldc_driver_stop_sends_emergency_stop);
    RUN_TEST(test_bldc_driver_get_position_and_ticks);
    RUN_TEST(test_bldc_driver_process_rx_updates_state);
    RUN_TEST(test_bldc_driver_process_rx_ignores_wrong_node_id);
    RUN_TEST(test_bldc_driver_null_guards);

    s_current_setup = test_tmc2209_driver_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_tmc2209_create_populates_interface);
    RUN_TEST(test_tmc2209_init_disables_motor_pin_high);
    RUN_TEST(test_tmc2209_enable_sets_pin_low);
    RUN_TEST(test_tmc2209_set_direction_normal_and_inverted);
    RUN_TEST(test_tmc2209_move_to_updates_position);
    RUN_TEST(test_tmc2209_home_resets_position);
    RUN_TEST(test_tmc2209_null_guards);

    s_current_setup = test_brake_driver_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_brake_create_populates_interface);
    RUN_TEST(test_brake_init_engages_brake);
    RUN_TEST(test_brake_release_sets_pin_high);
    RUN_TEST(test_brake_engage_sets_pin_low);
    RUN_TEST(test_brake_null_guards);

    s_current_setup = test_operator_input_driver_setUp;
    s_current_teardown = test_operator_input_driver_tearDown;
    RUN_TEST(test_operator_create_populates_interface);
    RUN_TEST(test_operator_compute_axis_norm_values);
    RUN_TEST(test_operator_read_axis_uses_buffer);
    RUN_TEST(test_operator_read_button_brake_active_low);
    RUN_TEST(test_operator_null_guards);

    printf("\n========================================\n");
    printf("  2. DOMAIN SERVICES TESTS             \n");
    printf("========================================\n");
    s_current_setup = test_motor_service_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_motor_service_initialization);
    RUN_TEST(test_motor_service_init_null_guards);
    RUN_TEST(test_motor_service_bind_driver);
    RUN_TEST(test_motor_service_enable_disable_single_and_all);
    RUN_TEST(test_motor_service_move_to_with_direction);
    RUN_TEST(test_motor_service_rotate_with_direction);
    RUN_TEST(test_motor_service_get_actual_position_and_encoder);
    RUN_TEST(test_motor_service_home_and_stop);
    RUN_TEST(test_motor_service_set_direction_with_null_driver_function);
    RUN_TEST(test_motor_service_set_direction_propagates_driver_failure);
    RUN_TEST(test_motor_service_get_position_fallback_when_driver_null);
    RUN_TEST(test_motor_service_get_position_fallback_when_getter_null);
    RUN_TEST(test_motor_service_get_encoder_fallback_when_driver_null_and_getter_null);
    RUN_TEST(test_motor_service_home_with_null_driver_and_null_home_fn);
    RUN_TEST(test_motor_service_rotate_all_null_guards);
    RUN_TEST(test_motor_service_move_all_null_guards);
    RUN_TEST(test_motor_service_rotate_all_propagates_failure);
    RUN_TEST(test_motor_service_move_all_propagates_failure);
    RUN_TEST(test_motor_service_stop_all_propagates_failure);
    RUN_TEST(test_motor_service_update_skips_null_driver);
    RUN_TEST(test_motor_service_update_skips_null_getter_functions);

    s_current_setup = test_brake_service_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_brake_service_init_success);
    RUN_TEST(test_brake_service_init_null_guards);
    RUN_TEST(test_brake_service_release_success);
    RUN_TEST(test_brake_service_engage_success);
    RUN_TEST(test_brake_service_get_state);

    s_current_setup = test_operator_service_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_operator_service_init_success);
    RUN_TEST(test_operator_service_deadband_filtering);
    RUN_TEST(test_operator_service_brake_button_detection);

    s_current_setup = test_cli_service_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_cli_service_init_null_guards);
    RUN_TEST(test_cli_service_init_success_full_services);
    RUN_TEST(test_cli_service_init_success_optional_services_null);
    RUN_TEST(test_cli_service_set_telemetry_enabled_toggle);
    RUN_TEST(test_cli_service_set_telemetry_enabled_null_guard);
    RUN_TEST(test_cli_process_help_command);
    RUN_TEST(test_cli_process_status_command);
    RUN_TEST(test_cli_process_stop_all_command);
    RUN_TEST(test_cli_process_brake_on_off);
    RUN_TEST(test_cli_process_log_on_off);
    RUN_TEST(test_cli_process_rotate_command);
    RUN_TEST(test_cli_process_move_command);
    RUN_TEST(test_cli_process_rotate_all_command);
    RUN_TEST(test_cli_process_move_all_command);
    RUN_TEST(test_cli_process_unknown_command);
    RUN_TEST(test_cli_process_backspace_removes_last_char);
    RUN_TEST(test_cli_process_backspace_0x7f_variant);
    RUN_TEST(test_cli_process_backspace_on_empty_buffer_noop);
    RUN_TEST(test_cli_process_rx_buffer_overflow_guard);
    RUN_TEST(test_cli_process_periodic_telemetry_emits_when_interval_elapsed);
    RUN_TEST(test_cli_process_periodic_telemetry_not_emitted_before_interval);
    RUN_TEST(test_cli_process_telemetry_disabled_no_output);
    RUN_TEST(test_cli_process_status_with_all_services_null);
    RUN_TEST(test_cli_process_stop_all_with_motor_svc_null);
    RUN_TEST(test_cli_process_brake_with_brake_svc_null);
    RUN_TEST(test_cli_process_rotate_with_motor_svc_null);
    RUN_TEST(test_cli_process_telemetry_with_services_null);

    printf("\n========================================\n");
    printf("  3. CONNECTIVITY (CLI PROTOCOL) TESTS \n");
    printf("========================================\n");
    s_current_setup = test_cli_protocol_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_cli_parse_help);
    RUN_TEST(test_cli_parse_status);
    RUN_TEST(test_cli_parse_stop_all);
    RUN_TEST(test_cli_parse_brake_on_off);
    RUN_TEST(test_cli_parse_rotate_single_axis);
    RUN_TEST(test_cli_parse_move_single_axis);
    RUN_TEST(test_cli_parse_rotate_all);
    RUN_TEST(test_cli_parse_move_all);
    RUN_TEST(test_cli_parse_log_on_off);
    RUN_TEST(test_cli_format_telemetry);
    RUN_TEST(test_cli_parse_null_and_invalid);
    RUN_TEST(test_cli_parse_empty_and_whitespace_only);
    RUN_TEST(test_cli_parse_too_long_string);
    RUN_TEST(test_cli_parse_brake_invalid_arg);
    RUN_TEST(test_cli_parse_log_invalid_arg);
    RUN_TEST(test_cli_parse_rotate_all_numeric_fallback_format);
    RUN_TEST(test_cli_parse_rotate_all_insufficient_args_fails);
    RUN_TEST(test_cli_parse_move_all_insufficient_args_fails);
    RUN_TEST(test_cli_parse_move_numeric_direction_format);
    RUN_TEST(test_cli_parse_move_numeric_direction_insufficient_args_fails);
    RUN_TEST(test_cli_format_telemetry_buffer_too_small);
    RUN_TEST(test_cli_format_telemetry_null_buffer);

    printf("\n========================================\n");
    printf("  4. APPLICATION COORDINATOR TESTS     \n");
    printf("========================================\n");
    s_current_setup = test_system_coordinator_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_system_coordinator_init_null_sys_returns_false);
    RUN_TEST(test_system_coordinator_init_success);
    RUN_TEST(test_system_coordinator_trigger_emergency_brake);
    RUN_TEST(test_system_coordinator_start_tasks_returns_true);

    printf("\n========================================\n");
    printf("  4b. RTOS TASK STEP TESTS (Emergency-Stop Path, Real Logic)  \n");
    printf("========================================\n");
    s_current_setup = test_rtos_tasks_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_input_scan_task_step_triggers_emergency_brake_on_button_press);
    RUN_TEST(test_input_scan_task_step_clears_flag_when_button_released);
    RUN_TEST(test_motion_control_task_step_stops_all_motors_when_brake_triggered);
    RUN_TEST(test_motion_control_task_step_rotates_motor_on_operator_demand);
    RUN_TEST(test_motion_control_task_step_stops_all_motors_when_idle);
    RUN_TEST(test_console_task_step_calls_cli_service_process_without_crash);
    RUN_TEST(test_rtos_notify_brake_event_is_safe_when_queue_not_yet_initialized);

    printf("\n========================================\n");
    printf("  5. MIDDLEWARE FREERTOS OSAL TESTS    \n");
    printf("========================================\n");
    s_current_setup = test_freertos_osal_setUp;
    s_current_teardown = NULL;
    RUN_TEST(test_osal_task_create_and_delete);
    RUN_TEST(test_osal_mutex_lock_unlock);
    RUN_TEST(test_osal_queue_send_receive);
    RUN_TEST(test_osal_timer_start_stop);
    RUN_TEST(test_osal_time_utilities);

    return UNITY_END();
}
