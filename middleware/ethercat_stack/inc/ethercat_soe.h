#ifndef ETHERCAT_SOE_H
#define ETHERCAT_SOE_H

#include "ethercat_app_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t idn;          // SERCOS IDN (e.g. S-0-0032 Target Position)
    uint16_t data_len;
    uint8_t  data[32];
} soe_element_t;

typedef struct {
    ecat_app_protocol_t proto;
    soe_element_t       elements[16];
    uint8_t             element_count;
} ethercat_soe_t;

bool ethercat_soe_init(ethercat_soe_t *soe);
ecat_app_protocol_t* ethercat_soe_get_protocol(ethercat_soe_t *soe);
bool ethercat_soe_set_idn(ethercat_soe_t *soe, uint16_t idn, const void *data, uint16_t len);
bool ethercat_soe_get_idn(const ethercat_soe_t *soe, uint16_t idn, void *data, uint16_t *len);

#ifdef __cplusplus
}
#endif

#endif // ETHERCAT_SOE_H
