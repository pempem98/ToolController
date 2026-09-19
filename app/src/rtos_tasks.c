#include <stddef.h>
#include <math.h>
#include "system_coordinator.h"
#include "rtos_tasks_config.h"
#include "freertos_osal.h"

/**
 * @brief Hàng đợi truyền sự kiện ngắt phanh khẩn cấp từ các Task hoặc ISR tới EmergencyBrakeTask.
 */
static osal_queue_t s_brake_evt_queue = NULL;

/**
 * @brief Gửi thông báo ngắt sự kiện phanh khẩn cấp vào hàng đợi FreeRTOS.
 * 
 * @details Hàm đẩy một byte tín hiệu (event = 1) vào hàng đợi s_brake_evt_queue với timeout = 0
 *          để đánh thức EmergencyBrakeTask ngay lập tức mà không gây block.
 * 
 * @return void
 * @note Hàm được thiết kế an toàn, không gây chặn luồng hiện tại.
 */
void rtos_notify_brake_event(void) {
    if (s_brake_evt_queue) {
        uint8_t evt = 1;
        osal_queue_send(s_brake_evt_queue, &evt, 0);
    }
}

// -----------------------------------------------------------------------------
// 1. Task Ngắt Phanh Khẩn Cấp (Ưu tiên cao nhất: < 1ms phản ứng)
// -----------------------------------------------------------------------------

/**
 * @brief Tác vụ FreeRTOS xử lý phanh an toàn khẩn cấp (Emergency Brake Task).
 * 
 * @details Tác vụ có mức độ ưu tiên cao nhất trong hệ thống (Realtime Priority).
 *          Chờ đợi sự kiện trên hàng đợi s_brake_evt_queue. Khi nhận được tín hiệu ngắt,
 *          nó lập tức kích hoạt khóa phanh vật lý (Brake Engage) và dừng toàn bộ 4 động cơ.
 * 
 * @param[in] pvParameters Con trỏ tham số truyền vào từ osal_task_create (không sử dụng).
 * 
 * @return void (Tác vụ RTOS chạy vô hạn).
 * @warning Tác vụ này quyết định tính an toàn cơ khí của hệ thống, không được chèn bất kỳ hàm delay nào.
 */
void EmergencyBrakeTask(void *pvParameters) {
    (void)pvParameters;
    if (!s_brake_evt_queue) {
        s_brake_evt_queue = osal_queue_create(1, sizeof(uint8_t));
    }

    uint8_t evt = 0;
    while (1) {
        if (osal_queue_receive(s_brake_evt_queue, &evt, 0xFFFFFFFF)) {
            brake_service_engage(&g_sys.brake_svc);
            motor_service_stop_all(&g_sys.motor_svc);
        }
    }
}

// -----------------------------------------------------------------------------
// 2. Task Quét Đầu Vào Người Vận Hành (Chu kỳ 10ms - Normal Priority)
// -----------------------------------------------------------------------------

/**
 * @brief Thực thi một bước đơn (Step) của tác vụ đọc và lọc tín hiệu người vận hành.
 * 
 * @details Đọc 4 kênh trục analog từ phần cứng (ADC/Chiết áp), áp dụng thuật toán
 *          lọc vùng chết (Deadband 5%), và kiểm tra trạng thái nút bấm phanh cơ khí.
 *          Nếu phát hiện nút phanh được nhấn, hàm sẽ kích hoạt ngắt phanh khẩn cấp.
 * 
 * @return void
 * @note Hàm này được tách riêng để có thể gọi trực tiếp trong các bài kiểm thử đơn vị (SIL Unit Test).
 */
void InputScanTaskStep(void) {
    operator_service_update(&g_sys.op_svc);

    if (operator_service_is_brake_requested(&g_sys.op_svc)) {
        if (!g_sys.emergency_brake_triggered) {
            system_coordinator_trigger_emergency_brake(&g_sys);
        }
    } else {
        g_sys.emergency_brake_triggered = false;
    }
}

/**
 * @brief Tác vụ FreeRTOS quét tín hiệu điều khiển người vận hành (Input Scan Task).
 * 
 * @details Chạy định kỳ theo chu kỳ 10ms (100 Hz). Thực hiện gọi InputScanTaskStep()
 *          sau đó nhường CPU bằng osal_delay_ms(10).
 * 
 * @param[in] pvParameters Tham số tác vụ (không sử dụng).
 * 
 * @return void (Tác vụ RTOS chạy vô hạn).
 */
void InputScanTask(void *pvParameters) {
    (void)pvParameters;
    while (1) {
        InputScanTaskStep();
        osal_delay_ms(10);
    }
}

// -----------------------------------------------------------------------------
// 3. Task Điều Khiển Đồng Thời Các Trục Động Cơ (Chu kỳ 10ms - High Priority)
// -----------------------------------------------------------------------------

/**
 * @brief Thực thi một bước đơn (Step) điều phối chuyển động đa trục động cơ.
 * 
 * @details 1. Kiểm tra trạng thái phanh khẩn cấp: nếu cờ phanh bật, lập tức dừng toàn bộ động cơ.
 *          2. Kiểm tra tín hiệu chuyển động từ người vận hành (operator_service_has_motion_demand).
 *          3. Nếu có yêu cầu chuyển động: Mở phanh an toàn, tính toán vận tốc và chiều quay
 *             cho từng trục độc lập theo giá trị normalized [-1.0f .. 1.0f], và ra lệnh quay.
 *             Nếu người dùng thả tay về 0 trên trục nào thì trục đó dừng quay.
 *          4. Nếu không có bất kỳ trục nào có lệnh: Dừng toàn bộ động cơ.
 *          5. Cập nhật trạng thái chu kỳ của motor_service_update().
 * 
 * @return void
 * @note Tách hàm phục vụ kiểm thử SIL (Software-in-the-Loop) độc lập.
 */
void MotionControlTaskStep(void) {
    if (g_sys.emergency_brake_triggered) {
        brake_service_engage(&g_sys.brake_svc);
        motor_service_stop_all(&g_sys.motor_svc);
        motor_service_update(&g_sys.motor_svc);
        return;
    }

    bool any_active = operator_service_has_motion_demand(&g_sys.op_svc);
    if (any_active) {
        brake_service_release(&g_sys.brake_svc);

        for (uint8_t i = 0; i < MOTOR_COUNT; i++) {
            float val = operator_service_get_axis(&g_sys.op_svc, i);
            if (fabsf(val) > 0.001f) {
                motor_direction_t dir = (val > 0.0f) ? MOTOR_DIR_CW : MOTOR_DIR_CCW;
                uint32_t speed = (uint32_t)(fabsf(val) * MAX_MOTOR_SPEED_RPM);
                motor_service_rotate(&g_sys.motor_svc, i, dir, speed);
            } else {
                motor_service_stop(&g_sys.motor_svc, i);
            }
        }
    } else {
        motor_service_stop_all(&g_sys.motor_svc);
    }

    motor_service_update(&g_sys.motor_svc);
}

/**
 * @brief Tác vụ FreeRTOS điều khiển chuyển động động cơ (Motion Control Task).
 * 
 * @details Chạy định kỳ với chu kỳ chính xác 10ms (100 Hz). Tác vụ có mức ưu tiên cao
 *          để đảm bảo phản ứng chuyển động mượt mà và đồng thời giữa các trục.
 * 
 * @param[in] pvParameters Tham số tác vụ (không sử dụng).
 * 
 * @return void (Tác vụ RTOS chạy vô hạn).
 */
void MotionControlTask(void *pvParameters) {
    (void)pvParameters;
    while (1) {
        MotionControlTaskStep();
        osal_delay_ms(10);
    }
}

// -----------------------------------------------------------------------------
// 4. Task Console UART Debug & Multi-Motor Test Command (Chu kỳ 50ms - Low Priority)
// -----------------------------------------------------------------------------

/**
 * @brief Thực thi một bước đơn (Step) của tác vụ giao diện dòng lệnh Console CLI.
 * 
 * @details Đọc tick hệ thống từ osal_get_tick_ms() và gọi hàm xử lý cli_service_process()
 *          để tiếp nhận ký tự từ cổng Console, parse lệnh ASCII và in định kỳ bản tin telemetry.
 * 
 * @return void
 */
void ConsoleTaskStep(void) {
    uint32_t tick = osal_get_tick_ms();
    cli_service_process(&g_sys.cli_svc, tick);
}

/**
 * @brief Tác vụ FreeRTOS quản lý cổng giao tiếp dòng lệnh và Telemetry (Console Task).
 * 
 * @details Chạy nền với mức ưu tiên thấp (Low Priority) và chu kỳ 50ms (20 Hz),
 *          không gây ảnh hưởng đến hiệu năng thời gian thực của tác vụ chuyển động và quét cảm biến.
 * 
 * @param[in] pvParameters Tham số tác vụ (không sử dụng).
 * 
 * @return void (Tác vụ RTOS chạy vô hạn).
 */
void ConsoleTask(void *pvParameters) {
    (void)pvParameters;
    while (1) {
        ConsoleTaskStep();
        osal_delay_ms(50);
    }
}
