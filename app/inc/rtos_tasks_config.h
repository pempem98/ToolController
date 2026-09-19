#ifndef RTOS_TASKS_CONFIG_H
#define RTOS_TASKS_CONFIG_H

/**
 * @file rtos_tasks_config.h
 * @brief Cấu hình phân tầng mức độ ưu tiên (Priority) và kích thước Stack của các FreeRTOS Task.
 */

/** @brief Task ngắt phanh khẩn cấp (Ưu tiên cao nhất, phản ứng tức thì < 1ms khi có sự kiện phanh) */
#define EMERGENCY_BRAKE_TASK_PRIORITY      7
#define EMERGENCY_BRAKE_TASK_STACK_SIZE    256

/** @brief Task điều khiển đồng thời các trục động cơ (Ưu tiên cao, chu kỳ đồng bộ 10ms - 100 Hz) */
#define MOTION_CONTROL_TASK_PRIORITY       5
#define MOTION_CONTROL_TASK_STACK_SIZE     512

/** @brief Task quét và lọc mảng chiết áp + nút phanh (Ưu tiên trung bình, chu kỳ 10ms) */
#define INPUT_SCAN_TASK_PRIORITY           4
#define INPUT_SCAN_TASK_STACK_SIZE         256

/** @brief Task Console debug log và nhận command qua serial (Ưu tiên thấp, chu kỳ 50ms) */
#define CONSOLE_TASK_PRIORITY              2
#define CONSOLE_TASK_STACK_SIZE            512

#endif // RTOS_TASKS_CONFIG_H
