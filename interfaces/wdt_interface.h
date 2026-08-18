#ifndef WDT_INTERFACE_H
#define WDT_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wdt_interface {
    bool (*init)(struct wdt_interface *self, uint32_t timeout_ms);
    void (*feed)(struct wdt_interface *self);
    void *priv_data;
} wdt_interface_t;

#ifdef __cplusplus
}
#endif

#endif // WDT_INTERFACE_H
