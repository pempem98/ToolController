#include "system_coordinator.h"
#include "rtos_tasks_config.h"
#include "freertos_osal.h"
#include <stddef.h>

// Forward declarations cho các RTOS task entries
extern void emergency_brake_task(void *pvParameters);
extern void motion_control_task(void *pvParameters);
extern void input_scan_task(void *pvParameters);
extern void console_task(void *pvParameters);
extern void rtos_notify_brake_event(void);

/**
 * @brief Thực thể Coordinator toàn cục quản lý trạng thái phối hợp của toàn bộ hệ thống.
 */
system_coordinator_t g_sys;

/**
 * @brief Khởi tạo toàn bộ bộ điều phối hệ thống và các dịch vụ nghiệp vụ (Domain Services).
 * 
 * @param[out] sys Con trỏ tới cấu trúc điều phối hệ thống system_coordinator_t cần khởi tạo.
 * @param[in]  hw  Con trỏ tới cấu trúc phần cứng board_hardware_t từ BSP.
 *                 Nếu truyền NULL, hàm sẽ tự động gọi board_get_hardware() để lấy cấu hình mặc định.
 * 
 * @return true Khởi tạo thành công toàn bộ domain services và liên kết driver.
 * @return false Khởi tạo thất bại do con trỏ không hợp lệ hoặc thiếu phần cứng board.
 * 
 * @note Hàm thực hiện bind động cơ, cấu hình vùng chết 5% cho bộ đọc joystick/chiết áp,
 *       cấu hình độ trễ tiếp điểm 50ms cho phanh cơ khí và khởi động dịch vụ CLI.
 */
bool system_coordinator_init(system_coordinator_t *sys, const board_hardware_t *hw) {
    if (!sys) return false;

    // 1. Phân giải Board hardware qua DI hoặc BSP contract
    if (!hw) {
        board_init();
        hw = board_get_hardware();
    }
    if (!hw) return false;

    sys->emergency_brake_triggered = false;

    // 2. Khởi tạo dịch vụ quản lý động cơ (Motor Service)
    motor_service_init(&sys->motor_svc, sys->motors, MOTOR_COUNT);
    for (uint8_t i = 0; i < hw->motor_count && i < MOTOR_COUNT; i++) {
        if (hw->motors[i]) {
            motor_service_bind_driver(&sys->motor_svc, i, hw->motors[i]);
        }
    }
    motor_service_enable_all(&sys->motor_svc);

    // 3. Khởi tạo dịch vụ người vận hành (vùng chết 5%) và kiểm soát phanh
    operator_service_init(&sys->op_svc, hw->operator_input, 0.05f);
    brake_service_init(&sys->brake_svc, hw->brake, 50);

    // 4. Khởi tạo CLI Service trên Console
    cli_service_init(&sys->cli_svc, hw->console, &sys->motor_svc, &sys->op_svc, &sys->brake_svc);

    return true;
}

/**
 * @brief Tạo và kích hoạt các tác vụ FreeRTOS chuyên biệt cho hệ thống.
 * 
 * @param[in,out] sys Con trỏ tới cấu trúc system_coordinator_t.
 * 
 * @return true Đã gửi yêu cầu tạo toàn bộ 4 task thành công tới FreeRTOS OSAL.
 * @return false Lỗi khi tạo một trong các task.
 * 
 * @note 4 tác vụ được khởi tạo theo phân tầng ưu tiên:
 *       1. emergency_brake_task (Priority: Realtime/High, xử lý ngắt phanh khẩn cấp < 1ms)
 *       2. motion_control_task (Priority: High, chu kỳ 10ms đồng bộ 4 trục)
 *       3. input_scan_task (Priority: Normal, chu kỳ 10ms quét chiết áp & nút bấm)
 *       4. console_task (Priority: Low, chu kỳ 50ms phục vụ UART CLI & Telemetry)
 */
bool system_coordinator_start_tasks(system_coordinator_t *sys) {
    (void)sys;
    // Khởi tạo các tasks trong FreeRTOS với độ ưu tiên phân tầng
    osal_task_create("EmgBrakeTask", emergency_brake_task, NULL, EMERGENCY_BRAKE_TASK_STACK_SIZE, EMERGENCY_BRAKE_TASK_PRIORITY);
    osal_task_create("MotionTask",   motion_control_task,  NULL, MOTION_CONTROL_TASK_STACK_SIZE,  MOTION_CONTROL_TASK_PRIORITY);
    osal_task_create("InputScanTask",input_scan_task,      NULL, INPUT_SCAN_TASK_STACK_SIZE,      INPUT_SCAN_TASK_PRIORITY);
    osal_task_create("ConsoleTask",  console_task,        NULL, CONSOLE_TASK_STACK_SIZE,         CONSOLE_TASK_PRIORITY);
    return true;
}

/**
 * @brief Kích hoạt sự kiện ngắt phanh khẩn cấp trên toàn hệ thống.
 * 
 * @param[in,out] sys Con trỏ tới cấu trúc system_coordinator_t.
 * 
 * @return void
 * 
 * @note Hàm này an toàn để gọi từ cả ngữ cảnh Task lẫn ISR (thông qua FreeRTOS queue/event).
 * @warning Ngay khi hàm này được gọi, cờ emergency_brake_triggered sẽ được bật và
 *          emergency_brake_task sẽ lập tức khóa phanh cơ khí và ra lệnh dừng khẩn cấp toàn bộ động cơ.
 */
void system_coordinator_trigger_emergency_brake(system_coordinator_t *sys) {
    if (!sys) return;
    osal_enter_critical();
    sys->emergency_brake_triggered = true;
    osal_exit_critical();
    rtos_notify_brake_event();
}
