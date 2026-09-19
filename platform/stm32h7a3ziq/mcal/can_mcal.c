#include "can_mcal.h"
#include <stddef.h>

static bool can_mcal_init(can_interface_t *self, uint32_t baudrate) {
    (void)self; (void)baudrate;
    return true;
}

static bool can_mcal_send(can_interface_t *self, const can_frame_t *frame) {
    (void)self; (void)frame;
    return true;
}

static bool can_mcal_receive(can_interface_t *self, can_frame_t *frame) {
    (void)self; (void)frame;
    return false;
}

static bool can_mcal_set_filter(can_interface_t *self, uint32_t id, uint32_t mask) {
    (void)self; (void)id; (void)mask;
    return true;
}

void can_mcal_create(can_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = can_mcal_init;
    adapter->send = can_mcal_send;
    adapter->receive = can_mcal_receive;
    adapter->set_filter = can_mcal_set_filter;
    adapter->priv_data = NULL;
}

