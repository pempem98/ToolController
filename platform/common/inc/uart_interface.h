#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "status_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Giao diện trừu tượng cho kênh truyền thông nối tiếp UART / USART
 */
typedef struct uart_interface {
    status_t (*init)(struct uart_interface *self, uint32_t baudrate);
    status_t (*send)(struct uart_interface *self, const uint8_t *data, size_t len);
    status_t (*receive)(struct uart_interface *self, uint8_t *buffer, size_t max_len, size_t *received_len);
    status_t (*set_baudrate)(struct uart_interface *self, uint32_t baudrate);
    void *priv_data;
} uart_interface_t;

#ifdef __cplusplus
}
#endif

#endif // UART_INTERFACE_H

