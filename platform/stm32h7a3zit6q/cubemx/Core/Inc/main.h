/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define JOY_X_Pin GPIO_PIN_2
#define JOY_X_GPIO_Port GPIOA
#define JOY_Y_Pin GPIO_PIN_3
#define JOY_Y_GPIO_Port GPIOA
#define JOY_SW_Pin GPIO_PIN_0
#define JOY_SW_GPIO_Port GPIOB
#define STEP_1_Pin GPIO_PIN_9
#define STEP_1_GPIO_Port GPIOE
#define STEP_2_Pin GPIO_PIN_11
#define STEP_2_GPIO_Port GPIOE
#define STEP_3_Pin GPIO_PIN_13
#define STEP_3_GPIO_Port GPIOE
#define STEP_4_Pin GPIO_PIN_14
#define STEP_4_GPIO_Port GPIOE
#define SERIAL_TX_Pin GPIO_PIN_10
#define SERIAL_TX_GPIO_Port GPIOB
#define SERIAL_RX_Pin GPIO_PIN_11
#define SERIAL_RX_GPIO_Port GPIOB
#define TMC_TX_Pin GPIO_PIN_14
#define TMC_TX_GPIO_Port GPIOB
#define TMC_RX_Pin GPIO_PIN_15
#define TMC_RX_GPIO_Port GPIOB
#define DIR_1_Pin GPIO_PIN_0
#define DIR_1_GPIO_Port GPIOD
#define DIR_2_Pin GPIO_PIN_1
#define DIR_2_GPIO_Port GPIOD
#define DIR_3_Pin GPIO_PIN_2
#define DIR_3_GPIO_Port GPIOD
#define DIR_4_Pin GPIO_PIN_3
#define DIR_4_GPIO_Port GPIOD
#define EN_Pin GPIO_PIN_4
#define EN_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
