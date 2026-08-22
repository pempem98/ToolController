#ifndef GPIO_MOCK_H
#define GPIO_MOCK_H

#include "gpio_interface.h"
#include <stddef.h>

static gpio_state_t g_mock_gpio_state = GPIO_STATE_LOW;

static bool mock_gpio_init(gpio_interface_t *self, gpio_mode_t mode) {
    (void)self; (void)mode;
    return true;
}

static void mock_gpio_write(gpio_interface_t *self, gpio_state_t state) {
    (void)self;
    g_mock_gpio_state = state;
}

static gpio_state_t mock_gpio_read(gpio_interface_t *self) {
    (void)self;
    return g_mock_gpio_state;
}

static void mock_gpio_toggle(gpio_interface_t *self) {
    (void)self;
    g_mock_gpio_state = (g_mock_gpio_state == GPIO_STATE_LOW) ? GPIO_STATE_HIGH : GPIO_STATE_LOW;
}

static bool mock_gpio_attach_interrupt(gpio_interface_t *self, gpio_irq_callback_t cb, void *arg) {
    (void)self; (void)cb; (void)arg;
    return true;
}

static inline void gpio_mock_create(gpio_interface_t *mock) {
    if (!mock) return;
    mock->init = mock_gpio_init;
    mock->write = mock_gpio_write;
    mock->read = mock_gpio_read;
    mock->toggle = mock_gpio_toggle;
    mock->attach_interrupt = mock_gpio_attach_interrupt;
    mock->priv_data = NULL;
}

#endif // GPIO_MOCK_H
