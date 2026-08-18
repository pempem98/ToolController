#include "brake_controller.h"
#include <stddef.h>

bool brake_controller_init(brake_controller_t *brake, gpio_interface_t *gpio, uint32_t delay_ms) {
    if (!brake || !gpio) return false;
    brake->gpio = gpio;
    brake->delay_ms = delay_ms;
    brake->state = BRAKE_STATE_ENGAGED;

    if (brake->gpio->init) {
        brake->gpio->init(brake->gpio, GPIO_MODE_OUTPUT);
    }
    brake->gpio->write(brake->gpio, GPIO_STATE_LOW); // Default safe engaged
    return true;
}

bool brake_release(brake_controller_t *brake) {
    if (!brake || !brake->gpio) return false;
    brake->gpio->write(brake->gpio, GPIO_STATE_HIGH);
    brake->state = BRAKE_STATE_RELEASED;
    return true;
}

bool brake_engage(brake_controller_t *brake) {
    if (!brake || !brake->gpio) return false;
    brake->gpio->write(brake->gpio, GPIO_STATE_LOW);
    brake->state = BRAKE_STATE_ENGAGED;
    return true;
}

brake_state_t brake_get_state(const brake_controller_t *brake) {
    if (!brake) return BRAKE_STATE_ENGAGED;
    return brake->state;
}
