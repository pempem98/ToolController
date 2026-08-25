#ifndef BOARD_ADAPTERS_H
#define BOARD_ADAPTERS_H

#include "can_interface.h"
#include "gpio_interface.h"
#include "wdt_interface.h"

void board_can_adapter_create(can_interface_t *adapter);
void board_gpio_adapter_create(gpio_interface_t *adapter);
void board_wdt_adapter_create(wdt_interface_t *adapter);

#endif // BOARD_ADAPTERS_H
