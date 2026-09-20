#include "operator_input_driver.h"
#include "main.h"
#include "adc.h"

// Buffer nhận dữ liệu ADC1 qua DMA trong vùng AXI SRAM (RAM) để DMA1 truy cập được
#if defined(__GNUC__)
static uint16_t s_adc_dma_raw[2] __attribute__((section(".dma_buffer"), aligned(32)));
#else
static uint16_t s_adc_dma_raw[2] __attribute__((aligned(32)));
#endif
static uint16_t *s_p_adc_buffer = s_adc_dma_raw;
static bool s_adc_started = false;

float operator_input_compute_axis_norm(uint16_t raw_val) {
    int32_t raw_diff = (int32_t)raw_val - 32768;
    float norm = (float)raw_diff / 32768.0f;
    if (norm > 1.0f) norm = 1.0f;
    if (norm < -1.0f) norm = -1.0f;
    return norm;
}

void operator_input_driver_set_buffer_override(uint16_t *custom_buffer) {
    s_p_adc_buffer = custom_buffer ? custom_buffer : s_adc_dma_raw;
}

static status_t stm32_operator_init(operator_input_interface_t *self) {
    if (!self) return STATUS_INVALID_PARAM;

    if (!s_adc_started) {
        HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
        s_adc_dma_raw[0] = 32768;
        s_adc_dma_raw[1] = 32768;
        if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)s_adc_dma_raw, 2) == HAL_OK) {
            s_adc_started = true;
        }
    }
    return STATUS_OK;
}

static status_t stm32_operator_read_axis(operator_input_interface_t *self, uint8_t axis, float *value) {
    (void)self;
    if (!value || axis >= OPERATOR_MAX_AXES) return STATUS_INVALID_PARAM;

    if (axis == OPERATOR_AXIS_0) {
        *value = operator_input_compute_axis_norm(s_p_adc_buffer[0]);
    } else if (axis == OPERATOR_AXIS_1) {
        *value = operator_input_compute_axis_norm(s_p_adc_buffer[1]);
    } else {
        *value = 0.0f;
    }

    return STATUS_OK;
}

static status_t stm32_operator_read_button(operator_input_interface_t *self, uint8_t button_id, bool *pressed) {
    (void)self;
    if (!pressed) return STATUS_INVALID_PARAM;

    if (button_id == OPERATOR_BTN_BRAKE) {
        GPIO_PinState pin_state = HAL_GPIO_ReadPin(JOY_SW_GPIO_Port, JOY_SW_Pin);
        *pressed = (pin_state == GPIO_PIN_RESET);
        return STATUS_OK;
    }

    *pressed = false;
    return STATUS_OK;
}

void operator_input_driver_create(operator_input_interface_t *adapter) {
    if (!adapter) return;
    adapter->init = stm32_operator_init;
    adapter->read_axis = stm32_operator_read_axis;
    adapter->read_button = stm32_operator_read_button;
    adapter->priv_data = NULL;
}

