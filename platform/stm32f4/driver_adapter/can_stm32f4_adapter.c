#include "can_interface.h"
#include "can.h"
#include <stddef.h>

static bool stm32f4_can_init(can_interface_t *self, uint32_t baudrate) {
    (void)self; (void)baudrate;
    // Call HAL_CAN_Init() & HAL_CAN_Start()
    return true;
}

static bool stm32f4_can_send(can_interface_t *self, const can_frame_t *frame) {
    (void)self; (void)frame;
    // Call HAL_CAN_AddTxMessage()
    return true;
}

static bool stm32f4_can_receive(can_interface_t *self, can_frame_t *frame) {
    (void)self; (void)frame;
    // Call HAL_CAN_GetRxMessage()
    return false;
}

static bool stm32f4_can_set_filter(can_interface_t *self, uint32_t id, uint32_t mask) {
    (void)self; (void)id; (void)mask;
    // Call HAL_CAN_ConfigFilter()
    return true;
}

void can_stm32f4_adapter_create(can_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = stm32f4_can_init;
    adapter->send = stm32f4_can_send;
    adapter->receive = stm32f4_can_receive;
    adapter->set_filter = stm32f4_can_set_filter;
    adapter->priv_data = NULL;
}
