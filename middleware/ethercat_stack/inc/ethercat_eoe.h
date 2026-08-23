#ifndef ETHERCAT_EOE_H
#define ETHERCAT_EOE_H

#include "ethercat_app_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ecat_app_protocol_t proto;
    uint8_t             mac_address[6];
    uint8_t             rx_frame_buf[1536];
    uint16_t            rx_frame_len;
    uint8_t             tx_frame_buf[1536];
    uint16_t            tx_frame_len;
} ethercat_eoe_t;

bool ethercat_eoe_init(ethercat_eoe_t *eoe, const uint8_t mac[6]);
ecat_app_protocol_t* ethercat_eoe_get_protocol(ethercat_eoe_t *eoe);
bool ethercat_eoe_send_frame(ethercat_eoe_t *eoe, const uint8_t *frame, uint16_t len);
bool ethercat_eoe_receive_frame(ethercat_eoe_t *eoe, uint8_t *frame, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif // ETHERCAT_EOE_H
