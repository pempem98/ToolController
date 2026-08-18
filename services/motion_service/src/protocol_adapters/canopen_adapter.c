#include "motor_protocol.h"
#include <stddef.h>

static bool canopen_init(motor_protocol_adapter_t *self) {
    (void)self;
    return true;
}

static bool canopen_send_target_position(motor_protocol_adapter_t *self, uint8_t node_id, int32_t pos) {
    (void)self; (void)node_id; (void)pos;
    return true;
}

static bool canopen_send_controlword(motor_protocol_adapter_t *self, uint8_t node_id, uint16_t controlword) {
    (void)self; (void)node_id; (void)controlword;
    return true;
}

static bool canopen_read_statusword(motor_protocol_adapter_t *self, uint8_t node_id, uint16_t *statusword) {
    (void)self; (void)node_id;
    if (statusword) *statusword = 0x0027;
    return true;
}

static bool canopen_read_actual_position(motor_protocol_adapter_t *self, uint8_t node_id, int32_t *pos) {
    (void)self; (void)node_id;
    if (pos) *pos = 0;
    return true;
}

void canopen_adapter_create(motor_protocol_adapter_t *adapter) {
    if (!adapter) return;
    adapter->type = MOTOR_PROTOCOL_CANOPEN;
    adapter->init = canopen_init;
    adapter->send_target_position = canopen_send_target_position;
    adapter->send_controlword = canopen_send_controlword;
    adapter->read_statusword = canopen_read_statusword;
    adapter->read_actual_position = canopen_read_actual_position;
    adapter->priv_data = NULL;
}
