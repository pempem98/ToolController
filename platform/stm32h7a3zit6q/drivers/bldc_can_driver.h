#ifndef BLDC_CAN_DRIVER_H
#define BLDC_CAN_DRIVER_H

#include "motor_interface.h"
#include "can_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t          node_id;
    can_interface_t *can_bus;
    int32_t          actual_position;
    int32_t          target_position;
    int32_t          encoder_ticks;
    int16_t          actual_speed;
    motor_direction_t current_dir;
    bool             enabled;
} bldc_can_motor_priv_t;

void bldc_can_driver_create(motor_interface_t *adapter,
                            bldc_can_motor_priv_t *priv,
                            uint8_t node_id,
                            can_interface_t *can_bus);

void bldc_can_driver_process_rx(motor_interface_t *adapter, const can_frame_t *frame);

#ifdef __cplusplus
}
#endif

#endif // BLDC_CAN_DRIVER_H

