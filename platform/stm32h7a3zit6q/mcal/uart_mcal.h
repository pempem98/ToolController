#ifndef UART_MCAL_H
#define UART_MCAL_H

#include "uart_interface.h"
#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    UART_HandleTypeDef *huart;
} stm32_uart_priv_t;

void uart_mcal_create(uart_interface_t *adapter, void *priv_storage, UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif // UART_MCAL_H

