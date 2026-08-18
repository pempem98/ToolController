#ifndef SENSOR_PROCESSOR_H
#define SENSOR_PROCESSOR_H

#include <stdint.h>
#include <stdbool.h>
#include "sensor_interface.h"
#include "gpio_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    sensor_interface_t *sensor;
    gpio_interface_t *endstop;
    float alpha; // Low-pass filter factor
    float filtered_force;
    bool endstop_triggered;
} sensor_processor_t;

bool sensor_processor_init(sensor_processor_t *sp, sensor_interface_t *sensor, gpio_interface_t *endstop, float alpha);
bool sensor_processor_update(sensor_processor_t *sp);
float sensor_processor_get_force(const sensor_processor_t *sp);
bool sensor_processor_is_endstop_hit(const sensor_processor_t *sp);

#ifdef __cplusplus
}
#endif

#endif // SENSOR_PROCESSOR_H
