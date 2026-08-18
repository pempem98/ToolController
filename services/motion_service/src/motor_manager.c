#include "motor_manager.h"
#include <stddef.h>

bool motor_manager_init(motor_manager_t *mgr, motor_instance_t *motor_array, uint8_t count, motor_protocol_adapter_t *protocol) {
    if (!mgr || !motor_array || count == 0 || !protocol) return false;

    mgr->motors = motor_array;
    mgr->motor_count = count;
    mgr->protocol = protocol;

    for (uint8_t i = 0; i < count; i++) {
        mgr->motors[i].node_id = i + 1;
        mgr->motors[i].target_position = 0;
        mgr->motors[i].actual_position = 0;
        mgr->motors[i].enabled = false;
        cia402_init(&mgr->motors[i].fsm);
    }

    if (mgr->protocol->init) {
        mgr->protocol->init(mgr->protocol);
    }

    return true;
}

bool motor_manager_enable_all(motor_manager_t *mgr) {
    if (!mgr || !mgr->motors || !mgr->protocol) return false;

    for (uint8_t i = 0; i < mgr->motor_count; i++) {
        motor_instance_t *m = &mgr->motors[i];
        cia402_process_event(&m->fsm, CIA402_CMD_SHUTDOWN);
        mgr->protocol->send_controlword(mgr->protocol, m->node_id, m->fsm.control_word);

        cia402_process_event(&m->fsm, CIA402_CMD_SWITCH_ON);
        mgr->protocol->send_controlword(mgr->protocol, m->node_id, m->fsm.control_word);

        cia402_process_event(&m->fsm, CIA402_CMD_ENABLE_OPERATION);
        mgr->protocol->send_controlword(mgr->protocol, m->node_id, m->fsm.control_word);

        m->enabled = true;
    }
    return true;
}

bool motor_manager_disable_all(motor_manager_t *mgr) {
    if (!mgr || !mgr->motors || !mgr->protocol) return false;

    for (uint8_t i = 0; i < mgr->motor_count; i++) {
        motor_instance_t *m = &mgr->motors[i];
        cia402_process_event(&m->fsm, CIA402_CMD_DISABLE_OPERATION);
        mgr->protocol->send_controlword(mgr->protocol, m->node_id, m->fsm.control_word);
        m->enabled = false;
    }
    return true;
}

bool motor_manager_set_target_position(motor_manager_t *mgr, uint8_t motor_idx, int32_t target_pos) {
    if (!mgr || motor_idx >= mgr->motor_count) return false;
    mgr->motors[motor_idx].target_position = target_pos;
    return true;
}

bool motor_manager_update(motor_manager_t *mgr) {
    if (!mgr || !mgr->motors || !mgr->protocol) return false;

    for (uint8_t i = 0; i < mgr->motor_count; i++) {
        motor_instance_t *m = &mgr->motors[i];
        if (m->enabled) {
            mgr->protocol->send_target_position(mgr->protocol, m->node_id, m->target_position);
        }
        uint16_t sw = 0;
        if (mgr->protocol->read_statusword(mgr->protocol, m->node_id, &sw)) {
            m->fsm.status_word = sw;
            m->fsm.current_state = cia402_parse_statusword(sw);
        }
        mgr->protocol->read_actual_position(mgr->protocol, m->node_id, &m->actual_position);
    }
    return true;
}
