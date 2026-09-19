#ifndef SYSTEM_COORDINATOR_H
#define SYSTEM_COORDINATOR_H

#include <stdbool.h>
#include "app_config.h"
#include "board_interface.h"
#include "motor_service.h"
#include "brake_service.h"
#include "operator_service.h"
#include "cli_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_MOTOR_SPEED_RPM 1000

/**
 * @brief Cấu trúc dữ liệu chính của bộ điều phối toàn hệ thống ToolController.
 * 
 * @details Tập hợp toàn bộ các service nghiệp vụ, mảng đối tượng động cơ,
 *          và cờ trạng thái ngắt an toàn khẩn cấp.
 */
typedef struct {
    motor_service_t    motor_svc;                 /**< Dịch vụ điều phối & đồng bộ động cơ đa trục */
    motor_instance_t   motors[MOTOR_COUNT];       /**< Mảng trạng thái và cấu hình từng động cơ */
    operator_service_t op_svc;                    /**< Dịch vụ xử lý tương tác người vận hành (Joystick/Potentiometer) */
    brake_service_t    brake_svc;                 /**< Dịch vụ kiểm soát đóng/ngắt phanh an toàn */
    cli_service_t      cli_svc;                   /**< Dịch vụ giao tiếp dòng lệnh và truyền thông telemetry */

    volatile bool      emergency_brake_triggered; /**< Cờ báo trạng thái ngắt phanh khẩn cấp */
} system_coordinator_t;

extern system_coordinator_t g_sys;

/**
 * @brief Khởi tạo toàn bộ hệ thống coordinator và các domain services.
 * 
 * @param[out] sys Con trỏ tới struct system_coordinator_t cần khởi tạo.
 * @param[in]  hw  Con trỏ tới phần cứng board_hardware_t từ tầng Platform.
 * 
 * @return true Khởi tạo thành công.
 * @return false Lỗi tham số hoặc lỗi khởi tạo driver.
 */
bool system_coordinator_init(system_coordinator_t *sys, const board_hardware_t *hw);

/**
 * @brief Khởi động các RTOS tasks chuyên biệt (InputScan, MotionControl, EmergencyBrake, Console).
 * 
 * @param[in,out] sys Con trỏ tới struct system_coordinator_t.
 * 
 * @return true Tạo task thành công.
 * @return false Tạo task thất bại.
 */
bool system_coordinator_start_tasks(system_coordinator_t *sys);

/**
 * @brief Kích hoạt sự kiện ngắt phanh khẩn cấp từ bất kỳ nguồn nào (ISR hoặc Task).
 * 
 * @param[in,out] sys Con trỏ tới struct system_coordinator_t.
 * 
 * @return void
 * @warning Khóa phanh cơ khí ngay lập tức và dừng khẩn cấp toàn bộ các trục động cơ.
 */
void system_coordinator_trigger_emergency_brake(system_coordinator_t *sys);

#ifdef __cplusplus
}
#endif

#endif // SYSTEM_COORDINATOR_H
