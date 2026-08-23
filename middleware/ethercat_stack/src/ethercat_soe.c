#include "ethercat_soe.h"
#include <string.h>

static bool soe_init_impl(ecat_app_protocol_t *self) {
    (void)self;
    return true;
}

static bool soe_process_mailbox_impl(ecat_app_protocol_t *self, const uint8_t *rx_buf, uint16_t rx_len, uint8_t *tx_buf, uint16_t *tx_len) {
    if (!self || !self->priv_data || !rx_buf || rx_len < 4 || !tx_buf || !tx_len) return false;

    ethercat_soe_t *soe = (ethercat_soe_t*)self->priv_data;

    // SoE Header: OpCode (byte 0), IDN (bytes 1-2)
    uint16_t idn = (uint16_t)rx_buf[1] | ((uint16_t)rx_buf[2] << 8);

    tx_buf[0] = rx_buf[0]; // Response OpCode
    tx_buf[1] = rx_buf[1];
    tx_buf[2] = rx_buf[2];

    uint16_t len = 0;
    if (ethercat_soe_get_idn(soe, idn, &tx_buf[4], &len)) {
        tx_buf[3] = (uint8_t)len;
        *tx_len = 4 + len;
        return true;
    }

    *tx_len = 4;
    tx_buf[3] = 0;
    return false;
}

static bool soe_process_pdo_impl(ecat_app_protocol_t *self, const uint8_t *rx_pdo, uint8_t *tx_pdo, uint16_t len) {
    if (!self || !self->priv_data) return false;
    ethercat_soe_t *soe = (ethercat_soe_t*)self->priv_data;

    if (rx_pdo && len >= 4) {
        // IDN 0x0024: Target Position (S-0-0036 in SERCOS)
        ethercat_soe_set_idn(soe, 36, rx_pdo, 4);
    }
    if (tx_pdo && len >= 4) {
        // IDN 0x0033: Position Actual (S-0-0051 in SERCOS)
        uint16_t out_len;
        ethercat_soe_get_idn(soe, 51, tx_pdo, &out_len);
    }

    return true;
}

bool ethercat_soe_init(ethercat_soe_t *soe) {
    if (!soe) return false;

    memset(soe, 0, sizeof(ethercat_soe_t));
    soe->proto.type = ECAT_PROTO_SOE;
    soe->proto.init = soe_init_impl;
    soe->proto.process_mailbox = soe_process_mailbox_impl;
    soe->proto.process_pdo = soe_process_pdo_impl;
    soe->proto.priv_data = soe;

    return true;
}

ecat_app_protocol_t* ethercat_soe_get_protocol(ethercat_soe_t *soe) {
    if (!soe) return NULL;
    return &soe->proto;
}

bool ethercat_soe_set_idn(ethercat_soe_t *soe, uint16_t idn, const void *data, uint16_t len) {
    if (!soe || !data || len == 0 || len > 32) return false;

    for (uint8_t i = 0; i < soe->element_count; i++) {
        if (soe->elements[i].idn == idn) {
            memcpy(soe->elements[i].data, data, len);
            soe->elements[i].data_len = len;
            return true;
        }
    }

    if (soe->element_count >= 16) return false;
    soe_element_t *elem = &soe->elements[soe->element_count++];
    elem->idn = idn;
    elem->data_len = len;
    memcpy(elem->data, data, len);
    return true;
}

bool ethercat_soe_get_idn(const ethercat_soe_t *soe, uint16_t idn, void *data, uint16_t *len) {
    if (!soe || !data || !len) return false;

    for (uint8_t i = 0; i < soe->element_count; i++) {
        if (soe->elements[i].idn == idn) {
            memcpy(data, soe->elements[i].data, soe->elements[i].data_len);
            *len = soe->elements[i].data_len;
            return true;
        }
    }
    return false;
}
