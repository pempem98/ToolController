#include "bsp_adapters.h"
#include <stdio.h>

typedef struct {
    int32_t mock_pos;
    float mock_force;
    uint16_t mock_adc;
} sim_sensor_priv_t;

static sim_sensor_priv_t g_sim_sensor_priv = {
    .mock_pos = 100,
    .mock_force = 12.5f,
    .mock_adc = 2048
};

static bool sensor_sim_init(sensor_interface_t *self) {
    (void)self;
    printf("[SIM SENSOR] Sensor initialized\n");
    return true;
}

static bool sensor_sim_read_position(sensor_interface_t *self, int32_t *pos_ticks) {
    (void)self;
    if (!pos_ticks) return false;
    *pos_ticks = g_sim_sensor_priv.mock_pos;
    return true;
}

static bool sensor_sim_read_force(sensor_interface_t *self, float *force_n) {
    (void)self;
    if (!force_n) return false;
    *force_n = g_sim_sensor_priv.mock_force;
    return true;
}

static bool sensor_sim_read_raw_adc(sensor_interface_t *self, uint16_t *adc_val) {
    (void)self;
    if (!adc_val) return false;
    *adc_val = g_sim_sensor_priv.mock_adc;
    return true;
}

void bsp_sensor_adapter_create(sensor_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = sensor_sim_init;
    adapter->read_position = sensor_sim_read_position;
    adapter->read_force = sensor_sim_read_force;
    adapter->read_raw_adc = sensor_sim_read_raw_adc;
    adapter->priv_data = &g_sim_sensor_priv;
}
