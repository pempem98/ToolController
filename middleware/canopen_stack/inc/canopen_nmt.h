#ifndef CANOPEN_NMT_H
#define CANOPEN_NMT_H

#include "canopen_types.h"
#include "can_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t             node_id;
    canopen_nmt_state_t state;
    uint32_t            last_heartbeat_tick;
    uint32_t            heartbeat_producer_time_ms;
    can_interface_t    *can_dev;
    bool                fd_enabled;
} canopen_nmt_t;

bool canopen_nmt_init(canopen_nmt_t *nmt, uint8_t node_id, can_interface_t *can_dev, bool fd_enabled);
bool canopen_nmt_send_command(canopen_nmt_t *nmt, uint8_t target_node_id, canopen_nmt_cmd_t cmd);
bool canopen_nmt_process_msg(canopen_nmt_t *nmt, const can_frame_t *frame);
bool canopen_nmt_send_heartbeat(canopen_nmt_t *nmt);
canopen_nmt_state_t canopen_nmt_get_state(const canopen_nmt_t *nmt);

#ifdef __cplusplus
}
#endif

#endif // CANOPEN_NMT_H
