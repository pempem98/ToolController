#include "gpio_interface.h"
#include "gpio.h"
#include <stddef.h>

static bool stm32f4_gpio_init(gpio_interface_t *self, gpio_mode_t mode) {
    (void)self; (void)mode;
    return true;
}

static void stm32f4_gpio_write(gpio_interface_t *self, gpio_state_t state) {
    (void)self; (void)state;
    // Call HAL_GPIO_WritePin()
}

static gpio_state_t stm32f4_gpio_read(gpio_interface_t *self) {
    (void)self;
    // Call HAL_GPIO_ReadPin()
    return GPIO_STATE_LOW;
}

static void stm32f4_gpio_toggle(gpio_interface_t *self) {
    (void)self;
    // Call HAL_GPIO_TogglePin()
}

static bool stm32f4_gpio_attach_interrupt(gpio_interface_t *self, gpio_irq_callback_t cb, void *arg) {
    (void)self; (void)cb; (void)arg;
    return true;
}

void gpio_stm32f4_adapter_create(gpio_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = stm32f4_gpio_init;
    adapter->write = stm32f4_gpio_write;
    adapter->read = stm32f4_gpio_read;
    adapter->toggle = stm32f4_gpio_toggle;
    adapter->attach_interrupt = stm32f4_gpio_attach_interrupt;
    adapter->priv_data = NULL;
}
