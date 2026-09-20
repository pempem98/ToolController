#include "stm32_hal_stub.h"
#include <string.h>

GPIO_TypeDef stub_GPIOA;
GPIO_TypeDef stub_GPIOB;
GPIO_TypeDef stub_GPIOC;
GPIO_TypeDef stub_GPIOD;
GPIO_TypeDef stub_GPIOE;

ADC_HandleTypeDef hadc1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

#define MAX_PIN_RECORDS 64

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    GPIO_PinState state;
} pin_record_t;

static pin_record_t s_pins[MAX_PIN_RECORDS];
static size_t s_pin_count = 0;

void hal_stub_reset(void) {
    memset(s_pins, 0, sizeof(s_pins));
    s_pin_count = 0;
}

void hal_stub_set_pin_state(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state) {
    for (size_t i = 0; i < s_pin_count; i++) {
        if (s_pins[i].port == port && s_pins[i].pin == pin) {
            s_pins[i].state = state;
            return;
        }
    }
    if (s_pin_count < MAX_PIN_RECORDS) {
        s_pins[s_pin_count].port = port;
        s_pins[s_pin_count].pin = pin;
        s_pins[s_pin_count].state = state;
        s_pin_count++;
    }
}

GPIO_PinState hal_stub_get_pin_state(GPIO_TypeDef *port, uint16_t pin) {
    for (size_t i = 0; i < s_pin_count; i++) {
        if (s_pins[i].port == port && s_pins[i].pin == pin) {
            return s_pins[i].state;
        }
    }
    return GPIO_PIN_RESET;
}

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    hal_stub_set_pin_state(GPIOx, GPIO_Pin, PinState);
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    return hal_stub_get_pin_state(GPIOx, GPIO_Pin);
}

HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef *hadc, uint32_t *pData, uint32_t Length) {
    (void)hadc;
    (void)pData;
    (void)Length;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADCEx_Calibration_Start(ADC_HandleTypeDef *hadc, uint32_t CalibrationMode, uint32_t SingleDiff) {
    (void)hadc;
    (void)CalibrationMode;
    (void)SingleDiff;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    (void)huart;
    (void)pData;
    (void)Size;
    (void)Timeout;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout) {
    (void)huart;
    (void)pData;
    (void)Size;
    (void)Timeout;
    return HAL_TIMEOUT;
}
