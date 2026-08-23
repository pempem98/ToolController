#include "bsp_adapters.h"
#include <stdio.h>

typedef struct {
    uint32_t timeout_ms;
    uint32_t feed_count;
    bool is_running;
} sim_wdt_priv_t;

static sim_wdt_priv_t g_sim_wdt_priv;

static bool wdt_sim_init(wdt_interface_t *self, uint32_t timeout_ms) {
    (void)self;
    g_sim_wdt_priv.timeout_ms = timeout_ms;
    g_sim_wdt_priv.feed_count = 0;
    g_sim_wdt_priv.is_running = true;
    printf("[SIM WDT] Initialized with timeout %u ms\n", (unsigned int)timeout_ms);
    return true;
}

static void wdt_sim_feed(wdt_interface_t *self) {
    (void)self;
    g_sim_wdt_priv.feed_count++;
}

void bsp_wdt_adapter_create(wdt_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = wdt_sim_init;
    adapter->feed = wdt_sim_feed;
    adapter->priv_data = &g_sim_wdt_priv;
}
