#include "brake_driver.h"

static status_t stm32_brake_init(brake_interface_t *self) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    return self->engage(self);
}

static status_t stm32_brake_engage(brake_interface_t *self) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    stm32_brake_priv_t *priv = (stm32_brake_priv_t*)self->priv_data;

    HAL_GPIO_WritePin(priv->port, priv->pin, GPIO_PIN_RESET);
    priv->is_currently_engaged = true;
    return STATUS_OK;
}

static status_t stm32_brake_release(brake_interface_t *self) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    stm32_brake_priv_t *priv = (stm32_brake_priv_t*)self->priv_data;

    HAL_GPIO_WritePin(priv->port, priv->pin, GPIO_PIN_SET);
    priv->is_currently_engaged = false;
    return STATUS_OK;
}

static status_t stm32_brake_is_engaged(brake_interface_t *self, bool *is_engaged) {
    if (!self || !self->priv_data || !is_engaged) return STATUS_INVALID_PARAM;
    stm32_brake_priv_t *priv = (stm32_brake_priv_t*)self->priv_data;

    *is_engaged = priv->is_currently_engaged;
    return STATUS_OK;
}

void stm32_brake_driver_create(brake_interface_t *adapter,
                              stm32_brake_priv_t *priv,
                              GPIO_TypeDef *port,
                              uint16_t pin) {
    if (!adapter || !priv) return;

    priv->port = port;
    priv->pin = pin;
    priv->is_currently_engaged = true;

    adapter->init = stm32_brake_init;
    adapter->engage = stm32_brake_engage;
    adapter->release = stm32_brake_release;
    adapter->is_engaged = stm32_brake_is_engaged;
    adapter->priv_data = priv;
}

