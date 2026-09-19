#include "uart_mcal.h"
#include <stddef.h>

static status_t stm32_uart_init(uart_interface_t *self, uint32_t baudrate) {
    (void)baudrate;
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    return STATUS_OK;
}

static status_t stm32_uart_send(uart_interface_t *self, const uint8_t *data, size_t len) {
    if (!self || !self->priv_data || !data || len == 0) return STATUS_INVALID_PARAM;
    stm32_uart_priv_t *priv = (stm32_uart_priv_t*)self->priv_data;

    HAL_StatusTypeDef st = HAL_UART_Transmit(priv->huart, (uint8_t*)data, (uint16_t)len, 100);
    return (st == HAL_OK) ? STATUS_OK : STATUS_ERROR;
}

static status_t stm32_uart_receive(uart_interface_t *self, uint8_t *buffer, size_t max_len, size_t *received_len) {
    if (!self || !self->priv_data || !buffer || max_len == 0) return STATUS_INVALID_PARAM;
    stm32_uart_priv_t *priv = (stm32_uart_priv_t*)self->priv_data;

    HAL_StatusTypeDef st = HAL_UART_Receive(priv->huart, buffer, 1, 0); // Non-blocking poll 0ms
    if (st == HAL_OK) {
        if (received_len) *received_len = 1;
        return STATUS_OK;
    }

    if (received_len) *received_len = 0;
    return STATUS_TIMEOUT;
}

static status_t stm32_uart_set_baudrate(uart_interface_t *self, uint32_t baudrate) {
    (void)self; (void)baudrate;
    return STATUS_OK;
}

void uart_mcal_create(uart_interface_t *adapter, void *priv_storage, UART_HandleTypeDef *huart) {
    if (!adapter || !priv_storage || !huart) return;
    stm32_uart_priv_t *priv = (stm32_uart_priv_t*)priv_storage;
    priv->huart = huart;

    adapter->init = stm32_uart_init;
    adapter->send = stm32_uart_send;
    adapter->receive = stm32_uart_receive;
    adapter->set_baudrate = stm32_uart_set_baudrate;
    adapter->priv_data = priv;
}

