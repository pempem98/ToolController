#ifndef ETHERCAT_APP_PROTOCOL_H
#define ETHERCAT_APP_PROTOCOL_H

#include "ethercat_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecat_app_protocol {
    ecat_protocol_type_t type;
    bool (*init)(struct ecat_app_protocol *self);
    bool (*process_mailbox)(struct ecat_app_protocol *self, const uint8_t *rx_buf, uint16_t rx_len, uint8_t *tx_buf, uint16_t *tx_len);
    bool (*process_pdo)(struct ecat_app_protocol *self, const uint8_t *rx_pdo, uint8_t *tx_pdo, uint16_t len);
    void *priv_data;
} ecat_app_protocol_t;

#ifdef __cplusplus
}
#endif

#endif // ETHERCAT_APP_PROTOCOL_H
