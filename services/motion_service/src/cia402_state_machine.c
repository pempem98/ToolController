#include "cia402_state_machine.h"

void cia402_init(cia402_handle_t *handle) {
    if (!handle) return;
    handle->current_state = CIA402_STATE_SWITCH_ON_DISABLED;
    handle->status_word = 0x0000;
    handle->control_word = 0x0000;
}

cia402_state_t cia402_parse_statusword(uint16_t statusword) {
    // Bits mask: xxxx xxxx x0xx 0000
    uint16_t state_bits = statusword & 0x006F;

    if ((state_bits & 0x004F) == 0x0000) return CIA402_STATE_NOT_READY_TO_SWITCH_ON;
    if ((state_bits & 0x004F) == 0x0040) return CIA402_STATE_SWITCH_ON_DISABLED;
    if ((state_bits & 0x006F) == 0x0021) return CIA402_STATE_READY_TO_SWITCH_ON;
    if ((state_bits & 0x006F) == 0x0023) return CIA402_STATE_SWITCHED_ON;
    if ((state_bits & 0x006F) == 0x0027) return CIA402_STATE_OPERATION_ENABLED;
    if ((state_bits & 0x006F) == 0x0007) return CIA402_STATE_QUICK_STOP_ACTIVE;
    if ((state_bits & 0x004F) == 0x000F) return CIA402_STATE_FAULT_REACTION_ACTIVE;
    if ((state_bits & 0x004F) == 0x0008) return CIA402_STATE_FAULT;

    return CIA402_STATE_NOT_READY_TO_SWITCH_ON;
}

uint16_t cia402_generate_controlword(cia402_handle_t *handle, cia402_command_t cmd) {
    if (!handle) return 0;
    uint16_t cw = handle->control_word;

    switch (cmd) {
        case CIA402_CMD_SHUTDOWN:
            cw = (cw & ~0x0087) | 0x0006;
            break;
        case CIA402_CMD_SWITCH_ON:
            cw = (cw & ~0x008F) | 0x0007;
            break;
        case CIA402_CMD_ENABLE_OPERATION:
            cw = (cw & ~0x008F) | 0x000F;
            break;
        case CIA402_CMD_DISABLE_OPERATION:
            cw = (cw & ~0x008F) | 0x0007;
            break;
        case CIA402_CMD_DISABLE_VOLTAGE:
            cw = (cw & ~0x0082);
            break;
        case CIA402_CMD_QUICK_STOP:
            cw = (cw & ~0x0084) | 0x0002;
            break;
        case CIA402_CMD_FAULT_RESET:
            cw |= 0x0080;
            break;
        default:
            break;
    }
    handle->control_word = cw;
    return cw;
}

bool cia402_process_event(cia402_handle_t *handle, cia402_command_t cmd) {
    if (!handle) return false;
    cia402_generate_controlword(handle, cmd);
    return true;
}
