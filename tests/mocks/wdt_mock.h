#ifndef WDT_MOCK_H
#define WDT_MOCK_H

#include "wdt_interface.h"
#include <stddef.h>

static uint32_t g_mock_wdt_init_count = 0;
static uint32_t g_mock_wdt_feed_count = 0;

static bool mock_wdt_init(wdt_interface_t *self, uint32_t timeout_ms) {
    (void)self; (void)timeout_ms;
    g_mock_wdt_init_count++;
    return true;
}

static void mock_wdt_feed(wdt_interface_t *self) {
    (void)self;
    g_mock_wdt_feed_count++;
}

static inline void wdt_mock_create(wdt_interface_t *mock) {
    if (!mock) return;
    mock->init = mock_wdt_init;
    mock->feed = mock_wdt_feed;
    mock->priv_data = NULL;
}

static inline void wdt_mock_reset(void) {
    g_mock_wdt_init_count = 0;
    g_mock_wdt_feed_count = 0;
}

#endif // WDT_MOCK_H
