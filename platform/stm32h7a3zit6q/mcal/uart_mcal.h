#ifndef UART_MCAL_H
#define UART_MCAL_H

#include "uart_interface.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UART_MCAL_RX_BUFFER_SIZE 256

typedef struct {
    UART_HandleTypeDef *huart;
    uint8_t rx_buffer[UART_MCAL_RX_BUFFER_SIZE];
    volatile uint16_t rx_head;
    volatile uint16_t rx_tail;
} stm32_uart_priv_t;

void uart_mcal_create(uart_interface_t *adapter, void *priv_storage, UART_HandleTypeDef *huart);
void uart_mcal_irq_handler(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif // UART_MCAL_H

