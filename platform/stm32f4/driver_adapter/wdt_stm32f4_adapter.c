#include "wdt_interface.h"
#include <stddef.h>

static bool stm32f4_wdt_init(wdt_interface_t *self, uint32_t timeout_ms) {
    (void)self; (void)timeout_ms;
    // Call HAL_IWDG_Init()
    return true;
}

static void stm32f4_wdt_feed(wdt_interface_t *self) {
    (void)self;
    // Call HAL_IWDG_Refresh()
}

void wdt_stm32f4_adapter_create(wdt_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = stm32f4_wdt_init;
    adapter->feed = stm32f4_wdt_feed;
    adapter->priv_data = NULL;
}
