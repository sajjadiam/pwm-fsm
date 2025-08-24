/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    iwdg.c
  * @brief   This file provides code for the configuration
  *          of the IWDG instances.
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
/* Includes ------------------------------------------------------------------*/
#include "iwdg.h"

/* USER CODE BEGIN 0 */
void IWDG_Start_F1(uint32_t timeout_ms, uint32_t f_lsi_hz){
  uint32_t presc = IWDG_PRESCALER_64;
  uint32_t reload = ((timeout_ms * f_lsi_hz) / (1000UL * 64UL)) - 1UL;
  if (reload > 4095UL){
    presc = IWDG_PRESCALER_256;
    reload = ((timeout_ms * f_lsi_hz) / (1000UL * 256UL)) - 1UL;
    if (reload > 4095UL) reload = 4095UL;
  }

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = presc;
  hiwdg.Init.Reload    = reload;

  /* توجه: در F1 معمولاً Freeze سخت‌افزاری IWDG در حالت Debug وجود ندارد.
     اگر ماکرو فراهم بود، از آن استفاده کن؛ وگرنه نمی‌شود متوقفش کرد. */
#ifdef __HAL_DBGMCU_FREEZE_IWDG
  __HAL_DBGMCU_FREEZE_IWDG();   // اگر تعریف شده بود
#endif

  HAL_IWDG_Init(&hiwdg);        // همینجا IWDG شروع به کار می‌کند
}

inline void IWDG_Kick(void){
  HAL_IWDG_Refresh(&hiwdg);
}
/* USER CODE END 0 */

IWDG_HandleTypeDef hiwdg;

/* IWDG init function */
void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */

  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */

  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
  hiwdg.Init.Reload = 625-1;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

/* USER CODE BEGIN 1 */


/* USER CODE END 1 */
