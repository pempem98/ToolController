#include "board_adapters.h"
#include "gpio.h"
#include <stddef.h>

static bool gpio_adapter_init(gpio_interface_t *self, gpio_mode_t mode) {
    (void)self; (void)mode;
    return true;
}

static void gpio_adapter_write(gpio_interface_t *self, gpio_state_t state) {
    (void)self; (void)state;
    // Call HAL_GPIO_WritePin()
}

static gpio_state_t gpio_adapter_read(gpio_interface_t *self) {
    (void)self;
    // Call HAL_GPIO_ReadPin()
    return GPIO_STATE_LOW;
}

static void gpio_adapter_toggle(gpio_interface_t *self) {
    (void)self;
    // Call HAL_GPIO_TogglePin()
}

static bool gpio_adapter_attach_interrupt(gpio_interface_t *self, gpio_irq_callback_t cb, void *arg) {
    (void)self; (void)cb; (void)arg;
    return true;
}

void board_gpio_adapter_create(gpio_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = gpio_adapter_init;
    adapter->write = gpio_adapter_write;
    adapter->read = gpio_adapter_read;
    adapter->toggle = gpio_adapter_toggle;
    adapter->attach_interrupt = gpio_adapter_attach_interrupt;
    adapter->priv_data = NULL;
}
