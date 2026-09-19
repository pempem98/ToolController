#ifndef MOTOR_SERVICE_H
#define MOTOR_SERVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "motor_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t            node_id;
    motor_interface_t  *driver;
    int32_t            target_position;
    int32_t            actual_position;
    int32_t            encoder_ticks;
    motor_direction_t  direction;
    bool               enabled;
} motor_instance_t;

typedef struct {
    motor_instance_t *motors;
    uint8_t           motor_count;
} motor_service_t;

bool motor_service_init(motor_service_t *svc, motor_instance_t *motor_array, uint8_t count);
bool motor_service_bind_driver(motor_service_t *svc, uint8_t motor_idx, motor_interface_t *driver);

bool motor_service_enable_all(motor_service_t *svc);
bool motor_service_disable_all(motor_service_t *svc);
bool motor_service_enable_motor(motor_service_t *svc, uint8_t motor_idx);
bool motor_service_disable_motor(motor_service_t *svc, uint8_t motor_idx);

bool motor_service_set_direction(motor_service_t *svc, uint8_t motor_idx, motor_direction_t dir);
bool motor_service_move_to(motor_service_t *svc, uint8_t motor_idx, int32_t target_pos, uint32_t speed, motor_direction_t dir);
bool motor_service_rotate(motor_service_t *svc, uint8_t motor_idx, motor_direction_t dir, uint32_t speed);
bool motor_service_get_actual_position(const motor_service_t *svc, uint8_t motor_idx, int32_t *pos);
bool motor_service_get_encoder(const motor_service_t *svc, uint8_t motor_idx, int32_t *ticks);

bool motor_service_stop(motor_service_t *svc, uint8_t motor_idx);
bool motor_service_stop_all(motor_service_t *svc);
bool motor_service_home(motor_service_t *svc, uint8_t motor_idx);

bool motor_service_rotate_all(motor_service_t *svc, const motor_direction_t *dirs, const uint32_t *speeds);
bool motor_service_move_all(motor_service_t *svc, const int32_t *target_positions, const uint32_t *speeds, const motor_direction_t *dirs);

bool motor_service_update(motor_service_t *svc);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_SERVICE_H

