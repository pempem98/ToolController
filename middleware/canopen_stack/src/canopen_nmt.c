#include "canopen_nmt.h"
#include <string.h>

bool canopen_nmt_init(canopen_nmt_t *nmt, uint8_t node_id, can_interface_t *can_dev, bool fd_enabled) {
    if (!nmt) return false;
    nmt->node_id = node_id;
    nmt->state = CANOPEN_NMT_STATE_PRE_OPERATIONAL;
    nmt->last_heartbeat_tick = 0;
    nmt->heartbeat_producer_time_ms = 1000;
    nmt->can_dev = can_dev;
    nmt->fd_enabled = fd_enabled;
    return true;
}

bool canopen_nmt_send_command(canopen_nmt_t *nmt, uint8_t target_node_id, canopen_nmt_cmd_t cmd) {
    if (!nmt || !nmt->can_dev || !nmt->can_dev->send) return false;

    can_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    frame.id = CANOPEN_COB_NMT;
    frame.len = 2;
    frame.data[0] = (uint8_t)cmd;
    frame.data[1] = target_node_id;
    frame.is_extended = false;
    frame.is_fd = nmt->fd_enabled;

    return nmt->can_dev->send(nmt->can_dev, &frame);
}

bool canopen_nmt_process_msg(canopen_nmt_t *nmt, const can_frame_t *frame) {
    if (!nmt || !frame) return false;

    // Check NMT service frame (COB-ID = 0x000)
    if (frame->id == CANOPEN_COB_NMT && frame->len >= 2) {
        uint8_t target = frame->data[1];
        if (target == 0 || target == nmt->node_id) {
            canopen_nmt_cmd_t cmd = (canopen_nmt_cmd_t)frame->data[0];
            switch (cmd) {
                case CANOPEN_NMT_CMD_START_NODE:
                    nmt->state = CANOPEN_NMT_STATE_OPERATIONAL;
                    break;
                case CANOPEN_NMT_CMD_STOP_NODE:
                    nmt->state = CANOPEN_NMT_STATE_STOPPED;
                    break;
                case CANOPEN_NMT_CMD_ENTER_PRE_OP:
                    nmt->state = CANOPEN_NMT_STATE_PRE_OPERATIONAL;
                    break;
                case CANOPEN_NMT_CMD_RESET_NODE:
                case CANOPEN_NMT_CMD_RESET_COMM:
                    nmt->state = CANOPEN_NMT_STATE_INITIALIZING;
                    break;
                default:
                    return false;
            }
            return true;
        }
    }

    return false;
}

bool canopen_nmt_send_heartbeat(canopen_nmt_t *nmt) {
    if (!nmt || !nmt->can_dev || !nmt->can_dev->send) return false;

    can_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    frame.id = CANOPEN_COB_NMT_HEARTBEAT_BASE + nmt->node_id;
    frame.len = 1;
    frame.data[0] = (uint8_t)nmt->state;
    frame.is_extended = false;
    frame.is_fd = nmt->fd_enabled;

    return nmt->can_dev->send(nmt->can_dev, &frame);
}

canopen_nmt_state_t canopen_nmt_get_state(const canopen_nmt_t *nmt) {
    if (!nmt) return CANOPEN_NMT_STATE_INITIALIZING;
    return nmt->state;
}
