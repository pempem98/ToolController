#ifndef CANOPEN_TYPES_H
#define CANOPEN_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// CANopen configuration (supporting both Classic CAN and CAN FD)
typedef struct {
    uint32_t nominal_baudrate; // Nominal baudrate (e.g. 1000000 = 1Mbps)
    uint32_t data_baudrate;    // Data phase bitrate for CAN FD (e.g. 2000000 = 2Mbps)
    bool     fd_enabled;       // True if CAN FD enabled
} canopen_can_config_t;

// Standard CANopen COB-IDs (CiA 301)
typedef enum {
    CANOPEN_COB_NMT       = 0x000,
    CANOPEN_COB_SYNC      = 0x080,
    CANOPEN_COB_EMCY_BASE = 0x080,
    CANOPEN_COB_TPDO1_BASE= 0x180,
    CANOPEN_COB_RPDO1_BASE= 0x200,
    CANOPEN_COB_TPDO2_BASE= 0x280,
    CANOPEN_COB_RPDO2_BASE= 0x300,
    CANOPEN_COB_TSDO_BASE = 0x580,
    CANOPEN_COB_RSDO_BASE = 0x600,
    CANOPEN_COB_NMT_HEARTBEAT_BASE = 0x700
} canopen_cob_base_t;

// NMT States
typedef enum {
    CANOPEN_NMT_STATE_INITIALIZING   = 0x00,
    CANOPEN_NMT_STATE_STOPPED        = 0x04,
    CANOPEN_NMT_STATE_OPERATIONAL    = 0x05,
    CANOPEN_NMT_STATE_PRE_OPERATIONAL= 0x7F
} canopen_nmt_state_t;

// NMT Commands
typedef enum {
    CANOPEN_NMT_CMD_START_NODE       = 0x01,
    CANOPEN_NMT_CMD_STOP_NODE        = 0x02,
    CANOPEN_NMT_CMD_ENTER_PRE_OP     = 0x80,
    CANOPEN_NMT_CMD_RESET_NODE       = 0x81,
    CANOPEN_NMT_CMD_RESET_COMM       = 0x82
} canopen_nmt_cmd_t;

#ifdef __cplusplus
}
#endif

#endif // CANOPEN_TYPES_H
