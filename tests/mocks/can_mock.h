#ifndef CAN_MOCK_H
#define CAN_MOCK_H

#include "can_interface.h"

static bool mock_can_init(can_interface_t *self, uint32_t baudrate) {
    (void)self; (void)baudrate;
    return true;
}

static bool mock_can_send(can_interface_t *self, const can_frame_t *frame) {
    (void)self; (void)frame;
    return true;
}

static bool mock_can_receive(can_interface_t *self, can_frame_t *frame) {
    (void)self; (void)frame;
    return false;
}

static bool mock_can_set_filter(can_interface_t *self, uint32_t id, uint32_t mask) {
    (void)self; (void)id; (void)mask;
    return true;
}

inline void can_mock_create(can_interface_t *mock) {
    if (!mock) return;
    mock->init = mock_can_init;
    mock->send = mock_can_send;
    mock->receive = mock_can_receive;
    mock->set_filter = mock_can_set_filter;
    mock->priv_data = nullptr;
}

#endif // CAN_MOCK_H
