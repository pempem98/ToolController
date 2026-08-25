#include "board_adapters.h"
#include "can.h"
#include <stddef.h>

static bool can_adapter_init(can_interface_t *self, uint32_t baudrate) {
    (void)self; (void)baudrate;
    // Call HAL_CAN_Init() & HAL_CAN_Start()
    return true;
}

static bool can_adapter_send(can_interface_t *self, const can_frame_t *frame) {
    (void)self; (void)frame;
    // Call HAL_CAN_AddTxMessage()
    return true;
}

static bool can_adapter_receive(can_interface_t *self, can_frame_t *frame) {
    (void)self; (void)frame;
    // Call HAL_CAN_GetRxMessage()
    return false;
}

static bool can_adapter_set_filter(can_interface_t *self, uint32_t id, uint32_t mask) {
    (void)self; (void)id; (void)mask;
    // Call HAL_CAN_ConfigFilter()
    return true;
}

void board_can_adapter_create(can_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = can_adapter_init;
    adapter->send = can_adapter_send;
    adapter->receive = can_adapter_receive;
    adapter->set_filter = can_adapter_set_filter;
    adapter->priv_data = NULL;
}
