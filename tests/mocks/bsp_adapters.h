#ifndef BSP_ADAPTERS_H
#define BSP_ADAPTERS_H

#include "can_mock.h"
#include "gpio_mock.h"
#include "wdt_mock.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void bsp_can_adapter_create(can_interface_t *adapter) {
    can_mock_create(adapter);
}

static inline void bsp_gpio_adapter_create(gpio_interface_t *adapter) {
    gpio_mock_create(adapter);
}

static inline void bsp_wdt_adapter_create(wdt_interface_t *adapter) {
    wdt_mock_create(adapter);
}

#ifdef __cplusplus
}
#endif

#endif // BSP_ADAPTERS_H
