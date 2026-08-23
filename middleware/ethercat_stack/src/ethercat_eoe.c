#include "ethercat_eoe.h"
#include <string.h>

static bool eoe_init_impl(ecat_app_protocol_t *self) {
    (void)self;
    return true;
}

static bool eoe_process_mailbox_impl(ecat_app_protocol_t *self, const uint8_t *rx_buf, uint16_t rx_len, uint8_t *tx_buf, uint16_t *tx_len) {
    if (!self || !self->priv_data || !rx_buf || rx_len < 4 || !tx_buf || !tx_len) return false;

    ethercat_eoe_t *eoe = (ethercat_eoe_t*)self->priv_data;

    // Store incoming Ethernet frame
    uint16_t copy_len = (rx_len - 4 < sizeof(eoe->rx_frame_buf)) ? (rx_len - 4) : sizeof(eoe->rx_frame_buf);
    memcpy(eoe->rx_frame_buf, &rx_buf[4], copy_len);
    eoe->rx_frame_len = copy_len;

    // Send outgoing Ethernet frame if available
    if (eoe->tx_frame_len > 0) {
        tx_buf[0] = 0x00; tx_buf[1] = 0x00; tx_buf[2] = 0x00; tx_buf[3] = 0x00; // EoE Header
        uint16_t send_len = (eoe->tx_frame_len < 1500) ? eoe->tx_frame_len : 1500;
        memcpy(&tx_buf[4], eoe->tx_frame_buf, send_len);
        *tx_len = 4 + send_len;
        eoe->tx_frame_len = 0; // Clear transmit buffer
        return true;
    }

    *tx_len = 0;
    return true;
}

static bool eoe_process_pdo_impl(ecat_app_protocol_t *self, const uint8_t *rx_pdo, uint8_t *tx_pdo, uint16_t len) {
    (void)self; (void)rx_pdo; (void)tx_pdo; (void)len;
    // EoE does not use PDO cyclic data
    return true;
}

bool ethercat_eoe_init(ethercat_eoe_t *eoe, const uint8_t mac[6]) {
    if (!eoe) return false;

    memset(eoe, 0, sizeof(ethercat_eoe_t));
    if (mac) {
        memcpy(eoe->mac_address, mac, 6);
    } else {
        uint8_t default_mac[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
        memcpy(eoe->mac_address, default_mac, 6);
    }

    eoe->proto.type = ECAT_PROTO_EOE;
    eoe->proto.init = eoe_init_impl;
    eoe->proto.process_mailbox = eoe_process_mailbox_impl;
    eoe->proto.process_pdo = eoe_process_pdo_impl;
    eoe->proto.priv_data = eoe;

    return true;
}

ecat_app_protocol_t* ethercat_eoe_get_protocol(ethercat_eoe_t *eoe) {
    if (!eoe) return NULL;
    return &eoe->proto;
}

bool ethercat_eoe_send_frame(ethercat_eoe_t *eoe, const uint8_t *frame, uint16_t len) {
    if (!eoe || !frame || len == 0 || len > sizeof(eoe->tx_frame_buf)) return false;
    memcpy(eoe->tx_frame_buf, frame, len);
    eoe->tx_frame_len = len;
    return true;
}

bool ethercat_eoe_receive_frame(ethercat_eoe_t *eoe, uint8_t *frame, uint16_t *len) {
    if (!eoe || !frame || !len || eoe->rx_frame_len == 0) return false;
    memcpy(frame, eoe->rx_frame_buf, eoe->rx_frame_len);
    *len = eoe->rx_frame_len;
    eoe->rx_frame_len = 0;
    return true;
}
