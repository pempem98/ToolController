#include "ethercat_slave.h"
#include <stddef.h>

bool ethercat_slave_init(ethercat_slave_t *slave, ethercat_interface_t *hw, ecat_app_protocol_t *app_proto) {
    if (!slave || !hw) return false;

    slave->hw = hw;
    slave->app_protocol = app_proto;

    if (!ethercat_al_init(&slave->al)) return false;

    if (slave->app_protocol && slave->app_protocol->init) {
        slave->app_protocol->init(slave->app_protocol);
    }

    return true;
}

bool ethercat_slave_set_protocol(ethercat_slave_t *slave, ecat_app_protocol_t *app_proto) {
    if (!slave) return false;
    slave->app_protocol = app_proto;
    if (slave->app_protocol && slave->app_protocol->init) {
        slave->app_protocol->init(slave->app_protocol);
    }
    return true;
}

bool ethercat_slave_process(ethercat_slave_t *slave) {
    if (!slave) return false;

    ethercat_al_update(&slave->al);

    ecat_al_state_t state = ethercat_al_get_state(&slave->al);

    if (state == ECAT_AL_STATE_OP || state == ECAT_AL_STATE_SAFE_OP) {
        if (slave->app_protocol && slave->app_protocol->process_pdo) {
            uint8_t tx_buf[64] = {0};
            uint8_t rx_buf[64] = {0};
            slave->app_protocol->process_pdo(slave->app_protocol, rx_buf, tx_buf, sizeof(tx_buf));
        }
    }

    return true;
}
