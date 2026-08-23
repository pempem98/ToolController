#ifndef BSP_ADAPTERS_H
#define BSP_ADAPTERS_H

#include "can_interface.h"
#include "gpio_interface.h"
#include "wdt_interface.h"
#include "sensor_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

void bsp_can_adapter_create(can_interface_t *adapter);
void bsp_gpio_adapter_create(gpio_interface_t *adapter);
void bsp_wdt_adapter_create(wdt_interface_t *adapter);
void bsp_sensor_adapter_create(sensor_interface_t *adapter);

#ifdef __cplusplus
}
#endif

#endif // BSP_ADAPTERS_H
