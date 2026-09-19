#ifndef BOARD_INTERFACE_H
#define BOARD_INTERFACE_H

#include "status_types.h"
#include "brake_interface.h"
#include "operator_input_interface.h"
#include "motor_interface.h"
#include "console_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_MAX_MOTORS 4

/**
 * @brief Hợp đồng phần cứng tổng hợp cung cấp bởi Board Support Package (BSP / IoHwAb).
 * Tầng Application và Domain Services hoàn toàn chỉ tương tác thông qua các đối tượng nghiệp vụ này,
 * không hề biết đến bất kỳ khái niệm chân cẳng, vi điều khiển hay ngoại vi cụ thể nào.
 */
typedef struct {
    brake_interface_t          *brake;          /**< Phanh an toàn cơ khí */
    operator_input_interface_t *operator_input; /**< Bảng điều khiển người vận hành (Trục điều khiển + Nút phanh) */
    motor_interface_t          *motors[BOARD_MAX_MOTORS]; /**< 4 driver động cơ độc lập */
    console_interface_t        *console;        /**< Kênh Console/Terminal phục vụ CLI và logging */
    uint8_t                     motor_count;    /**< Số lượng động cơ thực tế */
} board_hardware_t;

/**
 * @brief Khởi tạo toàn bộ cấu hình chân, ngoại vi và adapter của Board cụ thể.
 */
status_t board_init(void);

/**
 * @brief Lấy con trỏ cấu trúc phần cứng của Board hiện tại.
 */
const board_hardware_t* board_get_hardware(void);

#ifdef __cplusplus
}
#endif

#endif // BOARD_INTERFACE_H
