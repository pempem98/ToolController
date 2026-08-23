#ifndef ETHERCAT_SLAVE_H
#define ETHERCAT_SLAVE_H

#include "ethercat_interface.h"
#include "ethercat_al.h"
#include "ethercat_app_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    ethercat_interface_t *hw;
    ethercat_al_t         al;
    ecat_app_protocol_t  *app_protocol; // Pluggable (CoE, SoE, EoE, ...)
} ethercat_slave_t;

bool ethercat_slave_init(ethercat_slave_t *slave, ethercat_interface_t *hw, ecat_app_protocol_t *app_proto);
bool ethercat_slave_process(ethercat_slave_t *slave);
bool ethercat_slave_set_protocol(ethercat_slave_t *slave, ecat_app_protocol_t *app_proto);

#ifdef __cplusplus
}
#endif

#endif // ETHERCAT_SLAVE_H
