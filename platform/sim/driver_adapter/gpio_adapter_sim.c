#include "bsp_adapters.h"
#include <stdio.h>

typedef struct {
    gpio_state_t state;
    gpio_mode_t mode;
    gpio_irq_callback_t irq_cb;
    void *irq_arg;
} sim_gpio_priv_t;

static bool gpio_sim_init(gpio_interface_t *self, gpio_mode_t mode) {
    if (!self || !self->priv_data) return false;
    sim_gpio_priv_t *priv = (sim_gpio_priv_t*)self->priv_data;
    priv->mode = mode;
    priv->state = GPIO_STATE_LOW;
    printf("[SIM GPIO] Init mode: %d\n", mode);
    return true;
}

static void gpio_sim_write(gpio_interface_t *self, gpio_state_t state) {
    if (!self || !self->priv_data) return;
    sim_gpio_priv_t *priv = (sim_gpio_priv_t*)self->priv_data;
    priv->state = state;
    printf("[SIM GPIO] Write state: %s\n", state == GPIO_STATE_HIGH ? "HIGH" : "LOW");
}

static gpio_state_t gpio_sim_read(gpio_interface_t *self) {
    if (!self || !self->priv_data) return GPIO_STATE_LOW;
    sim_gpio_priv_t *priv = (sim_gpio_priv_t*)self->priv_data;
    return priv->state;
}

static void gpio_sim_toggle(gpio_interface_t *self) {
    if (!self || !self->priv_data) return;
    sim_gpio_priv_t *priv = (sim_gpio_priv_t*)self->priv_data;
    priv->state = (priv->state == GPIO_STATE_HIGH) ? GPIO_STATE_LOW : GPIO_STATE_HIGH;
    printf("[SIM GPIO] Toggled to: %s\n", priv->state == GPIO_STATE_HIGH ? "HIGH" : "LOW");
}

static bool gpio_sim_attach_interrupt(gpio_interface_t *self, gpio_irq_callback_t cb, void *arg) {
    if (!self || !self->priv_data) return false;
    sim_gpio_priv_t *priv = (sim_gpio_priv_t*)self->priv_data;
    priv->irq_cb = cb;
    priv->irq_arg = arg;
    printf("[SIM GPIO] Interrupt attached\n");
    return true;
}

static sim_gpio_priv_t g_sim_gpio_instances[4];
static uint8_t g_gpio_idx = 0;

void bsp_gpio_adapter_create(gpio_interface_t *adapter) {
    if (!adapter) return;
    uint8_t idx = g_gpio_idx % 4;
    g_gpio_idx++;
    g_sim_gpio_instances[idx].state = GPIO_STATE_LOW;
    g_sim_gpio_instances[idx].mode = GPIO_MODE_INPUT;
    g_sim_gpio_instances[idx].irq_cb = NULL;
    g_sim_gpio_instances[idx].irq_arg = NULL;

    adapter->init = gpio_sim_init;
    adapter->write = gpio_sim_write;
    adapter->read = gpio_sim_read;
    adapter->toggle = gpio_sim_toggle;
    adapter->attach_interrupt = gpio_sim_attach_interrupt;
    adapter->priv_data = &g_sim_gpio_instances[idx];
}
