#ifndef OPERATOR_SERVICE_H
#define OPERATOR_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "operator_input_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    operator_input_interface_t *hw_input;
    float                       raw_axes[OPERATOR_MAX_AXES];
    float                       filtered_axes[OPERATOR_MAX_AXES];
    float                       deadband;
    bool                        brake_requested;
} operator_service_t;

/**
 * @brief Khởi tạo dịch vụ xử lý tín hiệu từ người vận hành (Operator Service)
 */
bool operator_service_init(operator_service_t *svc, operator_input_interface_t *hw_input, float deadband);

/**
 * @brief Cập nhật định kỳ các trục điều khiển và nút bấm từ người vận hành
 */
bool operator_service_update(operator_service_t *svc);

/**
 * @brief Lấy giá trị trục điều khiển đã chuẩn hóa và lọc vùng chết [-1.0f .. 1.0f]
 */
float operator_service_get_axis(const operator_service_t *svc, uint8_t axis);

/**
 * @brief Kiểm tra xem người vận hành có đang yêu cầu phanh/dừng khẩn cấp hay không
 */
bool operator_service_is_brake_requested(const operator_service_t *svc);

/**
 * @brief Kiểm tra xem có bất kỳ trục nào đang yêu cầu chuyển động ngoài vùng chết
 */
bool operator_service_has_motion_demand(const operator_service_t *svc);

#ifdef __cplusplus
}
#endif

#endif // OPERATOR_SERVICE_H

