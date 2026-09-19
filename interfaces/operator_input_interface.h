#ifndef OPERATOR_INPUT_INTERFACE_H
#define OPERATOR_INPUT_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include "status_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OPERATOR_MAX_AXES    4
#define OPERATOR_MAX_BUTTONS 4

// Định danh các trục điều khiển của người vận hành
#define OPERATOR_AXIS_0      0
#define OPERATOR_AXIS_1      1
#define OPERATOR_AXIS_2      2
#define OPERATOR_AXIS_3      3

// Định danh các nút bấm an toàn / điều khiển
#define OPERATOR_BTN_BRAKE   0

/**
 * @brief Giao diện trừu tượng thu nhận tín hiệu từ Người Vận Hành (Operator Input)
 * Bất kể phần cứng bên dưới là chiết áp xoay, joystick công nghiệp, thanh trượt hay bàn phím,
 * tầng trên chỉ nhận các trục chuẩn hóa [-1.0f .. 1.0f] và trạng thái các nút bấm.
 */
typedef struct operator_input_interface {
    status_t (*init)(struct operator_input_interface *self);

    /**
     * @brief Đọc giá trị 1 trục điều khiển đã chuẩn hóa về dải [-1.0f .. 1.0f]
     * @param[in]  axis Chỉ số trục (0..OPERATOR_MAX_AXES - 1)
     * @param[out] value Độ lệch từ tâm (-1.0f: cực đại nghịch, 0.0f: vị trí nghỉ, +1.0f: cực đại thuận)
     */
    status_t (*read_axis)(struct operator_input_interface *self, uint8_t axis, float *value);

    /**
     * @brief Đọc trạng thái nút bấm từ bảng điều khiển người vận hành
     * @param[in]  button_id Mã nút bấm (ví dụ OPERATOR_BTN_BRAKE)
     * @param[out] pressed   true nếu nút đang được kích hoạt
     */
    status_t (*read_button)(struct operator_input_interface *self, uint8_t button_id, bool *pressed);

    void *priv_data;
} operator_input_interface_t;

#ifdef __cplusplus
}
#endif

#endif // OPERATOR_INPUT_INTERFACE_H

