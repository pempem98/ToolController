#include "canopen_pdo.h"
#include <string.h>

bool canopen_pdo_init(canopen_pdo_t *pdo, uint8_t node_id, can_interface_t *can_dev, bool fd_enabled) {
    if (!pdo) return false;
    pdo->node_id = node_id;
    pdo->can_dev = can_dev;
    pdo->fd_enabled = fd_enabled;

    for (int i = 0; i < CANOPEN_PDO_MAX_COUNT; i++) {
        pdo->maps[i].valid = false;
        pdo->maps[i].len = 0;
    }

    // Default COB-IDs according to CiA 301
    pdo->maps[CANOPEN_PDO_TYPE_RPDO1].cob_id = CANOPEN_COB_RPDO1_BASE + node_id;
    pdo->maps[CANOPEN_PDO_TYPE_RPDO1].valid = true;

    pdo->maps[CANOPEN_PDO_TYPE_TPDO1].cob_id = CANOPEN_COB_TPDO1_BASE + node_id;
    pdo->maps[CANOPEN_PDO_TYPE_TPDO1].valid = true;

    pdo->maps[CANOPEN_PDO_TYPE_RPDO2].cob_id = CANOPEN_COB_RPDO2_BASE + node_id;
    pdo->maps[CANOPEN_PDO_TYPE_RPDO2].valid = true;

    pdo->maps[CANOPEN_PDO_TYPE_TPDO2].cob_id = CANOPEN_COB_TPDO2_BASE + node_id;
    pdo->maps[CANOPEN_PDO_TYPE_TPDO2].valid = true;

    return true;
}

bool canopen_pdo_send_tpdo(canopen_pdo_t *pdo, canopen_pdo_type_t tpdo_num, const uint8_t *data, uint8_t len) {
    if (!pdo || !pdo->can_dev || !pdo->can_dev->send || !data || len == 0 || len > 64) return false;
    if (tpdo_num >= CANOPEN_PDO_MAX_COUNT || !pdo->maps[tpdo_num].valid) return false;

    can_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    frame.id = pdo->maps[tpdo_num].cob_id;
    frame.len = len;
    frame.is_extended = false;
    frame.is_fd = pdo->fd_enabled;
    memcpy(frame.data, data, len);

    return pdo->can_dev->send(pdo->can_dev, &frame);
}

bool canopen_pdo_process_rx(canopen_pdo_t *pdo, const can_frame_t *frame) {
    if (!pdo || !frame) return false;

    for (int i = 0; i < CANOPEN_PDO_MAX_COUNT; i++) {
        if (pdo->maps[i].valid && pdo->maps[i].cob_id == frame->id) {
            uint8_t copy_len = frame->len > 64 ? 64 : frame->len;
            memcpy(pdo->maps[i].data, frame->data, copy_len);
            pdo->maps[i].len = copy_len;
            return true;
        }
    }
    return false;
}

bool canopen_pdo_get_rpdo_data(const canopen_pdo_t *pdo, canopen_pdo_type_t rpdo_num, uint8_t *data, uint8_t *len) {
    if (!pdo || !data || !len || rpdo_num >= CANOPEN_PDO_MAX_COUNT) return false;
    if (!pdo->maps[rpdo_num].valid) return false;

    memcpy(data, pdo->maps[rpdo_num].data, pdo->maps[rpdo_num].len);
    *len = pdo->maps[rpdo_num].len;
    return true;
}
