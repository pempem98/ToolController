#ifndef MOTOR_PROTOCOL_H
#define MOTOR_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MOTOR_PROTOCOL_CANFD,
    MOTOR_PROTOCOL_CANOPEN,
    MOTOR_PROTOCOL_ETHERCAT_COE
} motor_protocol_type_t;

typedef struct motor_protocol_adapter {
    motor_protocol_type_t type;
    bool (*init)(struct motor_protocol_adapter *self);
    bool (*send_target_position)(struct motor_protocol_adapter *self, uint8_t node_id, int32_t pos);
    bool (*send_controlword)(struct motor_protocol_adapter *self, uint8_t node_id, uint16_t controlword);
    bool (*read_statusword)(struct motor_protocol_adapter *self, uint8_t node_id, uint16_t *statusword);
    bool (*read_actual_position)(struct motor_protocol_adapter *self, uint8_t node_id, int32_t *pos);
    void *priv_data;
} motor_protocol_adapter_t;

#ifdef __cplusplus
}
#endif

#endif // MOTOR_PROTOCOL_H
