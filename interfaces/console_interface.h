#ifndef CONSOLE_INTERFACE_H
#define CONSOLE_INTERFACE_H

#include <stdint.h>
#include <stddef.h>
#include "status_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Giao diện trừu tượng cho cổng Console / Terminal I/O của hệ thống.
 * Độc lập hoàn toàn với việc phần cứng thực tế dùng UART, USB CDC, RTT hay PC stdout.
 */
typedef struct console_interface {
    status_t (*write)(struct console_interface *self, const uint8_t *data, size_t len);
    status_t (*read)(struct console_interface *self, uint8_t *buffer, size_t max_len, size_t *received_len);
    void *priv_data;
} console_interface_t;

#ifdef __cplusplus
}
#endif

#endif // CONSOLE_INTERFACE_H

