#ifndef STM32_HAL_STUB_H
#define STM32_HAL_STUB_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

typedef enum {
    GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET
} GPIO_PinState;

typedef struct {
    uint32_t dummy;
} GPIO_TypeDef;

typedef struct {
    uint32_t dummy;
} UART_HandleTypeDef;

typedef struct {
    uint32_t dummy;
} ADC_HandleTypeDef;

// Mock GPIO ports
extern GPIO_TypeDef stub_GPIOA;
extern GPIO_TypeDef stub_GPIOB;
extern GPIO_TypeDef stub_GPIOD;
extern GPIO_TypeDef stub_GPIOE;

#define GPIOA (&stub_GPIOA)
#define GPIOB (&stub_GPIOB)
#define GPIOD (&stub_GPIOD)
#define GPIOE (&stub_GPIOE)

// Pin definitions
#define GPIO_PIN_0   ((uint16_t)0x0001U)
#define GPIO_PIN_1   ((uint16_t)0x0002U)
#define GPIO_PIN_2   ((uint16_t)0x0004U)
#define GPIO_PIN_3   ((uint16_t)0x0008U)
#define GPIO_PIN_4   ((uint16_t)0x0010U)
#define GPIO_PIN_9   ((uint16_t)0x0200U)
#define GPIO_PIN_10  ((uint16_t)0x0400U)
#define GPIO_PIN_11  ((uint16_t)0x0800U)
#define GPIO_PIN_13  ((uint16_t)0x2000U)
#define GPIO_PIN_14  ((uint16_t)0x4000U)
#define GPIO_PIN_15  ((uint16_t)0x8000U)

#define JOY_X_Pin             GPIO_PIN_2
#define JOY_X_GPIO_Port       GPIOA
#define JOY_Y_Pin             GPIO_PIN_3
#define JOY_Y_GPIO_Port       GPIOA
#define JOY_SW_Pin            GPIO_PIN_0
#define JOY_SW_GPIO_Port      GPIOB
#define STEP_1_Pin            GPIO_PIN_9
#define STEP_1_GPIO_Port      GPIOE
#define STEP_2_Pin            GPIO_PIN_11
#define STEP_2_GPIO_Port      GPIOE
#define SERIAL_TX_Pin         GPIO_PIN_10
#define SERIAL_TX_GPIO_Port   GPIOB
#define SERIAL_RX_Pin         GPIO_PIN_11
#define SERIAL_RX_GPIO_Port   GPIOB
#define TMC_TX_Pin            GPIO_PIN_14
#define TMC_TX_GPIO_Port      GPIOB
#define TMC_RX_Pin            GPIO_PIN_15
#define TMC_RX_GPIO_Port      GPIOB
#define DIR_1_Pin             GPIO_PIN_0
#define DIR_1_GPIO_Port       GPIOD
#define DIR_2_Pin             GPIO_PIN_1
#define DIR_2_GPIO_Port       GPIOD
#define EN_Pin                GPIO_PIN_4
#define EN_GPIO_Port          GPIOD

// Global HAL peripherals
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

// HAL API prototypes
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef *hadc, uint32_t *pData, uint32_t Length);
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);

// Test inspection helpers
void hal_stub_reset(void);
GPIO_PinState hal_stub_get_pin_state(GPIO_TypeDef *port, uint16_t pin);
void hal_stub_set_pin_state(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state);

#ifdef __cplusplus
}
#endif

#endif // STM32_HAL_STUB_H
