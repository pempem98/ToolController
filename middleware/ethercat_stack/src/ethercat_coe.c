#include "ethercat_coe.h"
#include <string.h>

static bool coe_init_impl(ecat_app_protocol_t *self) {
    (void)self;
    return true;
}

static bool coe_process_mailbox_impl(ecat_app_protocol_t *self, const uint8_t *rx_buf, uint16_t rx_len, uint8_t *tx_buf, uint16_t *tx_len) {
    if (!self || !self->priv_data || !rx_buf || rx_len < 8 || !tx_buf || !tx_len) return false;

    ethercat_coe_t *coe = (ethercat_coe_t*)self->priv_data;
    if (!coe->od) return false;

    // CoE Service Header: CoE Number (bits 12-15) | Service (bits 0-11)
    // SDO Header: Index (bytes 1-2), Subindex (byte 3)
    uint16_t index = (uint16_t)rx_buf[1] | ((uint16_t)rx_buf[2] << 8);
    uint8_t subindex = rx_buf[3];
    uint8_t command = rx_buf[0];

    tx_buf[0] = command;
    tx_buf[1] = rx_buf[1];
    tx_buf[2] = rx_buf[2];
    tx_buf[3] = subindex;

    uint8_t read_len = 0;
    if (canopen_od_read(coe->od, index, subindex, &tx_buf[4], &read_len)) {
        *tx_len = 4 + read_len;
        return true;
    }

    // Object not found response
    *tx_len = 8;
    tx_buf[4] = 0x00; tx_buf[5] = 0x00; tx_buf[6] = 0x02; tx_buf[7] = 0x06; // Abort code 0x06020000
    return false;
}

static bool coe_process_pdo_impl(ecat_app_protocol_t *self, const uint8_t *rx_pdo, uint8_t *tx_pdo, uint16_t len) {
    if (!self || !self->priv_data) return false;

    ethercat_coe_t *coe = (ethercat_coe_t*)self->priv_data;
    if (!coe->od) return false;

    if (rx_pdo && len >= 4) {
        // Map target position to OD
        canopen_od_write(coe->od, OD_IDX_TARGET_POSITION, 0, rx_pdo, 4);
    }

    if (tx_pdo && len >= 4) {
        // Map actual position from OD
        uint8_t dummy_len;
        canopen_od_read(coe->od, OD_IDX_POSITION_ACTUAL_VALUE, 0, tx_pdo, &dummy_len);
    }

    return true;
}

bool ethercat_coe_init(ethercat_coe_t *coe, canopen_od_t *od) {
    if (!coe || !od) return false;

    coe->od = od;
    coe->proto.type = ECAT_PROTO_COE;
    coe->proto.init = coe_init_impl;
    coe->proto.process_mailbox = coe_process_mailbox_impl;
    coe->proto.process_pdo = coe_process_pdo_impl;
    coe->proto.priv_data = coe;

    return true;
}

ecat_app_protocol_t* ethercat_coe_get_protocol(ethercat_coe_t *coe) {
    if (!coe) return NULL;
    return &coe->proto;
}
