#ifndef ETHERCAT_COE_H
#define ETHERCAT_COE_H

#include "ethercat_app_protocol.h"
#include "canopen_od.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ecat_app_protocol_t proto;
    canopen_od_t       *od;
} ethercat_coe_t;

bool ethercat_coe_init(ethercat_coe_t *coe, canopen_od_t *od);
ecat_app_protocol_t* ethercat_coe_get_protocol(ethercat_coe_t *coe);

#ifdef __cplusplus
}
#endif

#endif // ETHERCAT_COE_H
