#ifndef BRAKE_CONTROLLER_H
#define BRAKE_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "gpio_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BRAKE_STATE_ENGAGED,
    BRAKE_STATE_RELEASED,
    BRAKE_STATE_TRANSITIONING
} brake_state_t;

typedef struct {
    gpio_interface_t *gpio;
    uint32_t delay_ms;
    brake_state_t state;
} brake_controller_t;

bool brake_controller_init(brake_controller_t *brake, gpio_interface_t *gpio, uint32_t delay_ms);
bool brake_release(brake_controller_t *brake);
bool brake_engage(brake_controller_t *brake);
brake_state_t brake_get_state(const brake_controller_t *brake);

#ifdef __cplusplus
}
#endif

#endif // BRAKE_CONTROLLER_H
