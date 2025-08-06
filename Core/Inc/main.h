/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define RELAY_Pin GPIO_PIN_13
#define RELAY_GPIO_Port GPIOC
#define DIGIT4_Pin GPIO_PIN_4
#define DIGIT4_GPIO_Port GPIOA
#define DIGIT3_Pin GPIO_PIN_5
#define DIGIT3_GPIO_Port GPIOA
#define DIGIT2_Pin GPIO_PIN_6
#define DIGIT2_GPIO_Port GPIOA
#define DIGIT1_Pin GPIO_PIN_7
#define DIGIT1_GPIO_Port GPIOA
#define PWM_SS_Pin GPIO_PIN_0
#define PWM_SS_GPIO_Port GPIOB
#define SCREEN_C_Pin GPIO_PIN_1
#define SCREEN_C_GPIO_Port GPIOB
#define SEGMENT_G_Pin GPIO_PIN_10
#define SEGMENT_G_GPIO_Port GPIOB
#define SEGMENT_DP_Pin GPIO_PIN_11
#define SEGMENT_DP_GPIO_Port GPIOB
#define FAN1_Pin GPIO_PIN_12
#define FAN1_GPIO_Port GPIOA
#define FAN2_Pin GPIO_PIN_15
#define FAN2_GPIO_Port GPIOA
#define SEGMENT_A_Pin GPIO_PIN_4
#define SEGMENT_A_GPIO_Port GPIOB
#define SEGMENT_B_Pin GPIO_PIN_5
#define SEGMENT_B_GPIO_Port GPIOB
#define SEGMENT_C_Pin GPIO_PIN_6
#define SEGMENT_C_GPIO_Port GPIOB
#define SEGMENT_D_Pin GPIO_PIN_7
#define SEGMENT_D_GPIO_Port GPIOB
#define SEGMENT_E_Pin GPIO_PIN_8
#define SEGMENT_E_GPIO_Port GPIOB
#define SEGMENT_F_Pin GPIO_PIN_9
#define SEGMENT_F_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
