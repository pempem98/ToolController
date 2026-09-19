#ifndef MOTOR_INTERFACE_H
#define MOTOR_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include "status_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Hướng quay của động cơ
 */
typedef enum {
    MOTOR_DIR_CW   = 0, /**< Quay thuận / Theo chiều kim đồng hồ */
    MOTOR_DIR_CCW  = 1  /**< Quay nghịch / Ngược chiều kim đồng hồ */
} motor_direction_t;

/**
 * @brief Giao diện trừu tượng điều khiển động cơ (Stepper TMC2209, BLDC CAN 2.0B...)
 */
typedef struct motor_interface {
    status_t (*init)(struct motor_interface *self);
    status_t (*set_enabled)(struct motor_interface *self, bool enabled);
    status_t (*set_direction)(struct motor_interface *self, motor_direction_t dir);
    status_t (*move_to)(struct motor_interface *self, int32_t target_pos, uint32_t speed, motor_direction_t dir);
    status_t (*rotate)(struct motor_interface *self, motor_direction_t dir, uint32_t speed);
    status_t (*get_actual_position)(struct motor_interface *self, int32_t *pos);
    status_t (*get_encoder_ticks)(struct motor_interface *self, int32_t *ticks);
    status_t (*home)(struct motor_interface *self);
    status_t (*stop)(struct motor_interface *self);
    void *priv_data;
} motor_interface_t;

#ifdef __cplusplus
}
#endif

#endif // MOTOR_INTERFACE_H
