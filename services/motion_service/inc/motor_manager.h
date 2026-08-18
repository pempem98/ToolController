#ifndef MOTOR_MANAGER_H
#define MOTOR_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "cia402_state_machine.h"
#include "motor_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t node_id;
    cia402_handle_t fsm;
    int32_t target_position;
    int32_t actual_position;
    bool enabled;
} motor_instance_t;

typedef struct {
    motor_instance_t *motors;
    uint8_t motor_count;
    motor_protocol_adapter_t *protocol;
} motor_manager_t;

bool motor_manager_init(motor_manager_t *mgr, motor_instance_t *motor_array, uint8_t count, motor_protocol_adapter_t *protocol);
bool motor_manager_enable_all(motor_manager_t *mgr);
bool motor_manager_disable_all(motor_manager_t *mgr);
bool motor_manager_set_target_position(motor_manager_t *mgr, uint8_t motor_idx, int32_t target_pos);
bool motor_manager_update(motor_manager_t *mgr);

#ifdef __cplusplus
}
#endif

#endif // MOTOR_MANAGER_H
