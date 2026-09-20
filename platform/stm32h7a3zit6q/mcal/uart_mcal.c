#include "uart_mcal.h"
#include <stddef.h>
#include <string.h>

#define MAX_UART_INSTANCES 2
static stm32_uart_priv_t *s_uart_instances[MAX_UART_INSTANCES] = {NULL, NULL};

static status_t stm32_uart_init(uart_interface_t *self, uint32_t baudrate) {
    (void)baudrate;
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    stm32_uart_priv_t *priv = (stm32_uart_priv_t*)self->priv_data;

    priv->rx_head = 0;
    priv->rx_tail = 0;

    // Enable RX interrupt
    __HAL_UART_ENABLE_IT(priv->huart, UART_IT_RXNE);

    // Ensure NVIC interrupt is enabled for this UART
    if (priv->huart->Instance == USART3) {
        HAL_NVIC_SetPriority(USART3_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
    } else if (priv->huart->Instance == USART1) {
        HAL_NVIC_SetPriority(USART1_IRQn, 6, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }

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

    size_t count = 0;
    while (count < max_len && priv->rx_tail != priv->rx_head) {
        buffer[count++] = priv->rx_buffer[priv->rx_tail];
        priv->rx_tail = (uint16_t)((priv->rx_tail + 1) % UART_MCAL_RX_BUFFER_SIZE);
    }

    if (received_len) {
        *received_len = count;
    }

    return (count > 0) ? STATUS_OK : STATUS_TIMEOUT;
}

static status_t stm32_uart_set_baudrate(uart_interface_t *self, uint32_t baudrate) {
    (void)self; (void)baudrate;
    return STATUS_OK;
}

void uart_mcal_irq_handler(UART_HandleTypeDef *huart) {
    if (!huart) return;
    stm32_uart_priv_t *priv = NULL;
    for (size_t i = 0; i < MAX_UART_INSTANCES; i++) {
        if (s_uart_instances[i] && s_uart_instances[i]->huart == huart) {
            priv = s_uart_instances[i];
            break;
        }
    }
    if (!priv) return;

    // Clear Overrun Error
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE)) {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);
    }
    // Clear Framing, Noise, Parity Errors
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_FE)) {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_NE)) {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);
    }
    if (__HAL_UART_GET_FLAG(huart, UART_FLAG_PE)) {
        __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);
    }

    // Read all received bytes from hardware RDR register / FIFO
    while (__HAL_UART_GET_FLAG(huart, UART_FLAG_RXNE)) {
        uint8_t ch = (uint8_t)(huart->Instance->RDR & 0xFF);
        uint16_t next_head = (uint16_t)((priv->rx_head + 1) % UART_MCAL_RX_BUFFER_SIZE);
        if (next_head != priv->rx_tail) {
            priv->rx_buffer[priv->rx_head] = ch;
            priv->rx_head = next_head;
        }
    }
}

void uart_mcal_create(uart_interface_t *adapter, void *priv_storage, UART_HandleTypeDef *huart) {
    if (!adapter || !priv_storage || !huart) return;
    stm32_uart_priv_t *priv = (stm32_uart_priv_t*)priv_storage;
    priv->huart = huart;
    priv->rx_head = 0;
    priv->rx_tail = 0;
    memset(priv->rx_buffer, 0, sizeof(priv->rx_buffer));

    // Register instance for ISR routing
    for (size_t i = 0; i < MAX_UART_INSTANCES; i++) {
        if (s_uart_instances[i] == NULL || s_uart_instances[i] == priv) {
            s_uart_instances[i] = priv;
            break;
        }
    }

    adapter->init = stm32_uart_init;
    adapter->send = stm32_uart_send;
    adapter->receive = stm32_uart_receive;
    adapter->set_baudrate = stm32_uart_set_baudrate;
    adapter->priv_data = priv;
}

