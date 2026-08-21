#ifndef BSP_ADAPTERS_H
#define BSP_ADAPTERS_H

#include "can_interface.h"
#include "gpio_interface.h"
#include "wdt_interface.h"

void bsp_can_adapter_create(can_interface_t *adapter);
void bsp_gpio_adapter_create(gpio_interface_t *adapter);
void bsp_wdt_adapter_create(wdt_interface_t *adapter);

#endif // BSP_ADAPTERS_H
