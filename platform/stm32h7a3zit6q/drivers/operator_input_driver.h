#ifndef OPERATOR_INPUT_DRIVER_H
#define OPERATOR_INPUT_DRIVER_H

#include "operator_input_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Khởi tạo driver thu nhận tín hiệu người vận hành trên STM32H7A3
 *        - Trục 0: PA2 (ADC1_INP14) qua DMA1 Stream 0
 *        - Trục 1: PA3 (ADC1_INP15) qua DMA1 Stream 0
 *        - Trục 2, 3: Stub 0.0f
 *        - Nút phanh (OPERATOR_BTN_BRAKE): PB0 (JOY_SW_Pin) Pull-up (Active LOW)
 */
void operator_input_driver_create(operator_input_interface_t *adapter);

/**
 * @brief Chuyển đổi giá trị ADC raw 16-bit sang giá trị chuẩn hóa [-1.0f, +1.0f]
 */
float operator_input_compute_axis_norm(uint16_t raw_val);

/**
 * @brief Thiết lập buffer ADC override phục vụ kiểm thử (unit test / mock)
 */
void operator_input_driver_set_buffer_override(uint16_t *custom_buffer);

#ifdef __cplusplus
}
#endif

#endif // OPERATOR_INPUT_DRIVER_H

