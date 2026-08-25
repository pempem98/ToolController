#ifndef BOARD_ADAPTERS_H
#define BOARD_ADAPTERS_H

#include "can_mock.h"
#include "gpio_mock.h"
#include "wdt_mock.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void board_can_adapter_create(can_interface_t *adapter) {
    can_mock_create(adapter);
}

static inline void board_gpio_adapter_create(gpio_interface_t *adapter) {
    gpio_mock_create(adapter);
}

static inline void board_wdt_adapter_create(wdt_interface_t *adapter) {
    wdt_mock_create(adapter);
}

#ifdef __cplusplus
}
#endif

#endif // BOARD_ADAPTERS_H
