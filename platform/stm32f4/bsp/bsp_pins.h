#ifndef BSP_PINS_H
#define BSP_PINS_H

// Pin mapping for STM32F407VET6 board
#define BRAKE_1_PIN         GPIO_PIN_0
#define BRAKE_1_PORT        GPIOA

#define BRAKE_2_PIN         GPIO_PIN_1
#define BRAKE_2_PORT        GPIOA

#define ENDSTOP_1_PIN       GPIO_PIN_2
#define ENDSTOP_1_PORT      GPIOB

#define CAN1_RX_PIN         GPIO_PIN_11
#define CAN1_TX_PIN         GPIO_PIN_12
#define CAN1_PORT           GPIOA

#define ETHERCAT_SPI_CS_PIN GPIO_PIN_4
#define ETHERCAT_SPI_CS_PORT GPIOA

#endif // BSP_PINS_H
