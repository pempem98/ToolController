#ifndef CIA402_STATE_MACHINE_H
#define CIA402_STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CIA402_STATE_NOT_READY_TO_SWITCH_ON = 0,
    CIA402_STATE_SWITCH_ON_DISABLED,
    CIA402_STATE_READY_TO_SWITCH_ON,
    CIA402_STATE_SWITCHED_ON,
    CIA402_STATE_OPERATION_ENABLED,
    CIA402_STATE_QUICK_STOP_ACTIVE,
    CIA402_STATE_FAULT_REACTION_ACTIVE,
    CIA402_STATE_FAULT
} cia402_state_t;

typedef enum {
    CIA402_CMD_SHUTDOWN = 0,
    CIA402_CMD_SWITCH_ON,
    CIA402_CMD_ENABLE_OPERATION,
    CIA402_CMD_DISABLE_OPERATION,
    CIA402_CMD_DISABLE_VOLTAGE,
    CIA402_CMD_QUICK_STOP,
    CIA402_CMD_FAULT_RESET
} cia402_command_t;

typedef struct {
    cia402_state_t current_state;
    uint16_t status_word;
    uint16_t control_word;
} cia402_handle_t;

void cia402_init(cia402_handle_t *handle);
cia402_state_t cia402_parse_statusword(uint16_t statusword);
uint16_t cia402_generate_controlword(cia402_handle_t *handle, cia402_command_t cmd);
bool cia402_process_event(cia402_handle_t *handle, cia402_command_t cmd);

#ifdef __cplusplus
}
#endif

#endif // CIA402_STATE_MACHINE_H
