#include "tmc2209_driver.h"
#include <stdlib.h>

static status_t tmc2209_init(motor_interface_t *self) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    tmc2209_motor_priv_t *priv = (tmc2209_motor_priv_t*)self->priv_data;

    priv->current_steps = 0;
    priv->target_steps = 0;
    priv->current_dir = MOTOR_DIR_CW;
    priv->enabled = false;

    // TMC2209 Enable pin is active LOW: HIGH = Disabled
    if (priv->en_port) {
        HAL_GPIO_WritePin(priv->en_port, priv->en_pin, GPIO_PIN_SET);
    }

    // Set initial DIR
    if (priv->dir_port) {
        HAL_GPIO_WritePin(priv->dir_port, priv->dir_pin, priv->dir_inverted ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }

    return STATUS_OK;
}

static status_t tmc2209_set_enabled(motor_interface_t *self, bool enabled) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    tmc2209_motor_priv_t *priv = (tmc2209_motor_priv_t*)self->priv_data;
    priv->enabled = enabled;

    if (priv->en_port) {
        // Active LOW
        HAL_GPIO_WritePin(priv->en_port, priv->en_pin, enabled ? GPIO_PIN_RESET : GPIO_PIN_SET);
    }
    return STATUS_OK;
}

static status_t tmc2209_set_direction(motor_interface_t *self, motor_direction_t dir) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    tmc2209_motor_priv_t *priv = (tmc2209_motor_priv_t*)self->priv_data;
    priv->current_dir = dir;

    if (priv->dir_port) {
        GPIO_PinState pin_state = GPIO_PIN_RESET;
        if (dir == MOTOR_DIR_CW) {
            pin_state = priv->dir_inverted ? GPIO_PIN_RESET : GPIO_PIN_SET;
        } else {
            pin_state = priv->dir_inverted ? GPIO_PIN_SET : GPIO_PIN_RESET;
        }
        HAL_GPIO_WritePin(priv->dir_port, priv->dir_pin, pin_state);
    }
    return STATUS_OK;
}

static status_t tmc2209_move_to(motor_interface_t *self, int32_t target_pos, uint32_t speed, motor_direction_t dir) {
    (void)speed;
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    tmc2209_motor_priv_t *priv = (tmc2209_motor_priv_t*)self->priv_data;

    // 1. Immediately set the physical DIR pin
    tmc2209_set_direction(self, dir);

    // 2. Record target position
    priv->target_steps = target_pos;

    // 3. Update virtual position tracking (instantly simulated until Timer PWM DMA pulse stream is armed)
    priv->current_steps = target_pos;

    return STATUS_OK;
}

static status_t tmc2209_rotate(motor_interface_t *self, motor_direction_t dir, uint32_t speed) {
    (void)speed;
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    return tmc2209_set_direction(self, dir);
}

static status_t tmc2209_get_actual_position(motor_interface_t *self, int32_t *pos) {
    if (!self || !self->priv_data || !pos) return STATUS_INVALID_PARAM;
    tmc2209_motor_priv_t *priv = (tmc2209_motor_priv_t*)self->priv_data;
    *pos = priv->current_steps;
    return STATUS_OK;
}

static status_t tmc2209_get_encoder_ticks(motor_interface_t *self, int32_t *ticks) {
    return tmc2209_get_actual_position(self, ticks);
}

static status_t tmc2209_home(motor_interface_t *self) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    tmc2209_motor_priv_t *priv = (tmc2209_motor_priv_t*)self->priv_data;
    priv->current_steps = 0;
    priv->target_steps = 0;
    return STATUS_OK;
}

static status_t tmc2209_stop(motor_interface_t *self) {
    if (!self || !self->priv_data) return STATUS_INVALID_PARAM;
    return STATUS_OK;
}

void tmc2209_driver_create(motor_interface_t *adapter, 
                           tmc2209_motor_priv_t *priv,
                           uint8_t axis_id,
                           GPIO_TypeDef *step_port, uint16_t step_pin,
                           GPIO_TypeDef *dir_port, uint16_t dir_pin,
                           GPIO_TypeDef *en_port, uint16_t en_pin,
                           uart_interface_t *uart,
                           bool dir_inverted) {
    if (!adapter || !priv) return;

    priv->axis_id = axis_id;
    priv->step_port = step_port;
    priv->step_pin = step_pin;
    priv->dir_port = dir_port;
    priv->dir_pin = dir_pin;
    priv->en_port = en_port;
    priv->en_pin = en_pin;
    priv->uart = uart;
    priv->dir_inverted = dir_inverted;
    priv->current_dir = MOTOR_DIR_CW;
    priv->current_steps = 0;
    priv->target_steps = 0;
    priv->enabled = false;

    adapter->init = tmc2209_init;
    adapter->set_enabled = tmc2209_set_enabled;
    adapter->set_direction = tmc2209_set_direction;
    adapter->move_to = tmc2209_move_to;
    adapter->rotate = tmc2209_rotate;
    adapter->get_actual_position = tmc2209_get_actual_position;
    adapter->get_encoder_ticks = tmc2209_get_encoder_ticks;
    adapter->home = tmc2209_home;
    adapter->stop = tmc2209_stop;
    adapter->priv_data = priv;
}

