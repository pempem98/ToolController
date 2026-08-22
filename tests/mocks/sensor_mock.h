#ifndef SENSOR_MOCK_H
#define SENSOR_MOCK_H

#include "sensor_interface.h"
#include <stddef.h>

static float g_mock_sensor_force_val = 0.0f;
static int32_t g_mock_sensor_pos_val = 0;
static uint16_t g_mock_sensor_adc_val = 0;
static bool g_mock_sensor_read_force_ret = true;

static bool mock_sensor_init(sensor_interface_t *self) {
    (void)self;
    return true;
}

static bool mock_sensor_read_position(sensor_interface_t *self, int32_t *pos_ticks) {
    (void)self;
    if (pos_ticks) *pos_ticks = g_mock_sensor_pos_val;
    return true;
}

static bool mock_sensor_read_force(sensor_interface_t *self, float *force_n) {
    (void)self;
    if (!g_mock_sensor_read_force_ret) return false;
    if (force_n) *force_n = g_mock_sensor_force_val;
    return true;
}

static bool mock_sensor_read_raw_adc(sensor_interface_t *self, uint16_t *adc_val) {
    (void)self;
    if (adc_val) *adc_val = g_mock_sensor_adc_val;
    return true;
}

static inline void sensor_mock_create(sensor_interface_t *mock) {
    if (!mock) return;
    mock->init = mock_sensor_init;
    mock->read_position = mock_sensor_read_position;
    mock->read_force = mock_sensor_read_force;
    mock->read_raw_adc = mock_sensor_read_raw_adc;
    mock->priv_data = NULL;
}

static inline void sensor_mock_reset(void) {
    g_mock_sensor_force_val = 0.0f;
    g_mock_sensor_pos_val = 0;
    g_mock_sensor_adc_val = 0;
    g_mock_sensor_read_force_ret = true;
}

#endif // SENSOR_MOCK_H
