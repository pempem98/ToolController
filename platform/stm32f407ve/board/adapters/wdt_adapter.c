#include "board_adapters.h"
#include <stddef.h>

static bool wdt_adapter_init(wdt_interface_t *self, uint32_t timeout_ms) {
    (void)self; (void)timeout_ms;
    // Call HAL_IWDG_Init()
    return true;
}

static void wdt_adapter_feed(wdt_interface_t *self) {
    (void)self;
    // Call HAL_IWDG_Refresh()
}

void board_wdt_adapter_create(wdt_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = wdt_adapter_init;
    adapter->feed = wdt_adapter_feed;
    adapter->priv_data = NULL;
}
