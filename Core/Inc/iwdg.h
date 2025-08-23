/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    iwdg.h
  * @brief   This file contains all the function prototypes for
  *          the iwdg.c file
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
#ifndef __IWDG_H__
#define __IWDG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern IWDG_HandleTypeDef hiwdg;

/* USER CODE BEGIN Private defines */
typedef struct {
  uint8_t pin;     // ریست با پین
  uint8_t por_pdr; // Power-on / Power-down / Brown-out (برای F1 همه زیر PORRSTF)
  uint8_t sft;     // نرم‌افزاری
  uint8_t iwdg;    // IWDG
  uint8_t wwdg;    // WWDG
  uint8_t lpwr;    // Low-power reset
} ResetFlag_t;

static inline ResetFlag_t Read_Reset_Cause_F1(void){
  ResetFlag_t f = {0};
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))  f.pin     = 1;
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))  f.por_pdr = 1;   // شامل BOR هم می‌شود
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))  f.sft     = 1;
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) f.iwdg    = 1;
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) f.wwdg    = 1;
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) f.lpwr    = 1;
	__HAL_RCC_CLEAR_RESET_FLAGS(); // حتماً بعد از خواندن، پاک کن
  return f;
}
/* USER CODE END Private defines */

void MX_IWDG_Init(void);

/* USER CODE BEGIN Prototypes */
static void IWDG_Start_F1(uint32_t timeout_ms, uint32_t f_lsi_hz /*~40000*/);
static inline void IWDG_Kick(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __IWDG_H__ */

