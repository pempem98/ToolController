#include "canopen_sdo.h"
#include <string.h>

bool canopen_sdo_init(canopen_sdo_t *sdo, uint8_t node_id, can_interface_t *can_dev, bool fd_enabled) {
    if (!sdo) return false;
    sdo->node_id = node_id;
    sdo->can_dev = can_dev;
    sdo->fd_enabled = fd_enabled;
    return true;
}

bool canopen_sdo_write_expedited(canopen_sdo_t *sdo, uint16_t index, uint8_t subindex, const void *data, uint8_t len) {
    if (!sdo || !sdo->can_dev || !sdo->can_dev->send || !data || len == 0 || len > 64) return false;

    can_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    frame.id = CANOPEN_COB_RSDO_BASE + sdo->node_id;
    frame.is_extended = false;
    frame.is_fd = sdo->fd_enabled;

    if (len <= 4) {
        // Standard expedited 8-byte frame
        frame.len = 8;
        uint8_t n = 4 - len; // unused bytes
        frame.data[0] = (SDO_CCS_DOWNLOAD_INITIATE << 5) | (n << 2) | 0x03; // e=1, s=1
        frame.data[1] = (uint8_t)(index & 0xFF);
        frame.data[2] = (uint8_t)((index >> 8) & 0xFF);
        frame.data[3] = subindex;
        memcpy(&frame.data[4], data, len);
    } else {
        // CAN FD extended payload expedited write
        frame.len = 4 + len;
        if (frame.len < 8) frame.len = 8;
        frame.data[0] = (SDO_CCS_DOWNLOAD_INITIATE << 5) | 0x01; // e=0, s=1
        frame.data[1] = (uint8_t)(index & 0xFF);
        frame.data[2] = (uint8_t)((index >> 8) & 0xFF);
        frame.data[3] = subindex;
        memcpy(&frame.data[4], data, len);
    }

    return sdo->can_dev->send(sdo->can_dev, &frame);
}

bool canopen_sdo_read_expedited(canopen_sdo_t *sdo, uint16_t index, uint8_t subindex) {
    if (!sdo || !sdo->can_dev || !sdo->can_dev->send) return false;

    can_frame_t frame;
    memset(&frame, 0, sizeof(frame));
    frame.id = CANOPEN_COB_RSDO_BASE + sdo->node_id;
    frame.len = 8;
    frame.is_extended = false;
    frame.is_fd = sdo->fd_enabled;

    frame.data[0] = (SDO_CCS_UPLOAD_INITIATE << 5);
    frame.data[1] = (uint8_t)(index & 0xFF);
    frame.data[2] = (uint8_t)((index >> 8) & 0xFF);
    frame.data[3] = subindex;

    return sdo->can_dev->send(sdo->can_dev, &frame);
}

bool canopen_sdo_parse_response(const can_frame_t *frame, uint16_t *index, uint8_t *subindex, void *data, uint8_t *len, uint32_t *abort_code) {
    if (!frame || frame->len < 4) return false;

    uint8_t scs = (frame->data[0] >> 5) & 0x07;
    uint16_t idx = (uint16_t)frame->data[1] | ((uint16_t)frame->data[2] << 8);
    uint8_t subidx = frame->data[3];

    if (index) *index = idx;
    if (subindex) *subindex = subidx;

    if (scs == SDO_CCS_ABORT_TRANSFER) {
        if (abort_code && frame->len >= 8) {
            *abort_code = (uint32_t)frame->data[4] | ((uint32_t)frame->data[5] << 8) |
                          ((uint32_t)frame->data[6] << 16) | ((uint32_t)frame->data[7] << 24);
        }
        return false;
    }

    if (scs == SDO_CCS_UPLOAD_INITIATE) {
        bool e = (frame->data[0] & 0x02) != 0;
        bool s = (frame->data[0] & 0x01) != 0;
        uint8_t data_len = 4;
        if (e && s) {
            uint8_t n = (frame->data[0] >> 2) & 0x03;
            data_len = 4 - n;
        } else if (frame->len > 8) {
            data_len = frame->len - 4;
        }

        if (len) *len = data_len;
        if (data && frame->len >= 4 + data_len) {
            memcpy(data, &frame->data[4], data_len);
        }
        return true;
    }

    if (scs == SDO_CCS_DOWNLOAD_INITIATE) {
        if (len) *len = 0;
        return true;
    }

    return false;
}
