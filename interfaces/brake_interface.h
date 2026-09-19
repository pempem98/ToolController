#ifndef BRAKE_INTERFACE_H
#define BRAKE_INTERFACE_H

#include <stdbool.h>
#include "status_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Giao diện trừu tượng cho phanh cơ khí / phanh an toàn (Domain Actuator)
 * Tầng Application và Service hoàn toàn không biết phanh điều khiển bằng GPIO, CAN hay PWM.
 */
typedef struct brake_interface {
    status_t (*init)(struct brake_interface *self);

    /**
     * @brief Kích hoạt phanh (Khóa cơ khí / Safe state)
     */
    status_t (*engage)(struct brake_interface *self);

    /**
     * @brief Mở phanh (Cho phép động cơ chuyển động)
     */
    status_t (*release)(struct brake_interface *self);

    /**
     * @brief Kiểm tra trạng thái hiện tại của phanh
     * @param[out] is_engaged true nếu phanh đang bị khóa
     */
    status_t (*is_engaged)(struct brake_interface *self, bool *is_engaged);

    void *priv_data;
} brake_interface_t;

#ifdef __cplusplus
}
#endif

#endif // BRAKE_INTERFACE_H

