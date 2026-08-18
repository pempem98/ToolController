#ifndef SENSOR_INTERFACE_H
#define SENSOR_INTERFACE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sensor_interface {
    bool (*init)(struct sensor_interface *self);
    bool (*read_position)(struct sensor_interface *self, int32_t *pos_ticks);
    bool (*read_force)(struct sensor_interface *self, float *force_n);
    bool (*read_raw_adc)(struct sensor_interface *self, uint16_t *adc_val);
    void *priv_data;
} sensor_interface_t;

#ifdef __cplusplus
}
#endif

#endif // SENSOR_INTERFACE_H
