#include "sensor_processor.h"
#include <stddef.h>

bool sensor_processor_init(sensor_processor_t *sp, sensor_interface_t *sensor, gpio_interface_t *endstop, float alpha) {
    if (!sp) return false;
    sp->sensor = sensor;
    sp->endstop = endstop;
    sp->alpha = alpha;
    sp->filtered_force = 0.0f;
    sp->endstop_triggered = false;
    return true;
}

bool sensor_processor_update(sensor_processor_t *sp) {
    if (!sp) return false;

    if (sp->sensor && sp->sensor->read_force) {
        float raw_force = 0.0f;
        if (sp->sensor->read_force(sp->sensor, &raw_force)) {
            // Exponential moving average filter
            sp->filtered_force = (sp->alpha * raw_force) + ((1.0f - sp->alpha) * sp->filtered_force);
        }
    }

    if (sp->endstop && sp->endstop->read) {
        sp->endstop_triggered = (sp->endstop->read(sp->endstop) == GPIO_STATE_HIGH);
    }

    return true;
}

float sensor_processor_get_force(const sensor_processor_t *sp) {
    if (!sp) return 0.0f;
    return sp->filtered_force;
}

bool sensor_processor_is_endstop_hit(const sensor_processor_t *sp) {
    if (!sp) return false;
    return sp->endstop_triggered;
}
