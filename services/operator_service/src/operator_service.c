#include "operator_service.h"
#include <math.h>
#include <stddef.h>

bool operator_service_init(operator_service_t *svc, operator_input_interface_t *hw_input, float deadband) {
    if (!svc || !hw_input) return false;

    svc->hw_input = hw_input;
    svc->deadband = (deadband >= 0.0f && deadband < 1.0f) ? deadband : 0.05f;
    svc->brake_requested = false;

    for (uint8_t i = 0; i < OPERATOR_MAX_AXES; i++) {
        svc->raw_axes[i] = 0.0f;
        svc->filtered_axes[i] = 0.0f;
    }

    if (hw_input->init) {
        hw_input->init(hw_input);
    }
    return true;
}

bool operator_service_update(operator_service_t *svc) {
    if (!svc || !svc->hw_input) return false;

    for (uint8_t i = 0; i < OPERATOR_MAX_AXES; i++) {
        float val = 0.0f;
        if (svc->hw_input->read_axis) {
            svc->hw_input->read_axis(svc->hw_input, i, &val);
        }
        svc->raw_axes[i] = val;

        if (fabsf(val) <= svc->deadband) {
            svc->filtered_axes[i] = 0.0f;
        } else {
            if (val > 0.0f) {
                svc->filtered_axes[i] = (val - svc->deadband) / (1.0f - svc->deadband);
            } else {
                svc->filtered_axes[i] = (val + svc->deadband) / (1.0f - svc->deadband);
            }
        }
    }

    if (svc->hw_input->read_button) {
        bool pressed = false;
        if (svc->hw_input->read_button(svc->hw_input, OPERATOR_BTN_BRAKE, &pressed) == STATUS_OK) {
            svc->brake_requested = pressed;
        }
    }

    return true;
}

float operator_service_get_axis(const operator_service_t *svc, uint8_t axis) {
    if (!svc || axis >= OPERATOR_MAX_AXES) return 0.0f;
    return svc->filtered_axes[axis];
}

bool operator_service_is_brake_requested(const operator_service_t *svc) {
    if (!svc) return false;
    return svc->brake_requested;
}

bool operator_service_has_motion_demand(const operator_service_t *svc) {
    if (!svc) return false;
    for (uint8_t i = 0; i < OPERATOR_MAX_AXES; i++) {
        if (fabsf(svc->filtered_axes[i]) > 0.001f) {
            return true;
        }
    }
    return false;
}

