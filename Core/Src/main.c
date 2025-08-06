/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "key.h"
#include "pwm.h"
#include "sevenseg.h"
#include "value_to_string.h"
#include "mechanical_part.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


//----------------------------pwm defines ended----------------------------------
#define KEY_READ_PERIOD					50
#define SOFT_START_READ_PERIOD	2
#define STATE_NUM								7
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char buffer[6] = "     ";
PWM_Event_t evt;
const State_Machine_Func stateFunc[PwmStateEND] = {
	[PwmStateStandby]        	=	stateStandby ,																																	
	[PwmStateInit]           	=	stateInit ,																																	
	[PwmStateSoftStart]      	=	stateSoftStart ,
	[PwmStateResonanceSweep]	= stateResonanceSweep ,
	[PwmStateRunning]        	= stateRunning ,
	[PwmStateRecovery]        = stateRecovery ,
	[PwmStateSoftStop]       	= stateSoftStop ,
	[PwmStateHardStop]				= stateHardStop
};
static Counter keyCounter =0;
static FLAG keyRead = false;
static FLAG hardFaultFlag = false;
static FLAG sevenSegUpdateFlag = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
//-----------------------------pwm function prototype start--------------------------
void PWM_Command_Key_init(volatile KeyPinConfig* key);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	Mechnical_Part_Init(mechParts,MECHANICAL_PARTS_END);
	PWM_FSM_Init();
	PWM_Command_Key_init(&keys[PWM_COMMMAND_KEY]);
//	Mechnical_Part_Handler(&mechParts[MECHANICAL_PART_Relay],POWER_MODE_ON); اين براي سافت استارت خود باي ديسي يادم باشه درستش کنم 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//	pwm_init(&pwmState);
	SevenSeg_HandleTypeDef h7seg;
	SevenSeg_Init(&h7seg);
  while (1)
  {
		if(hardFaultFlag){
			hardFaultFlag = false;
			PWM_FSM_HandleEvent(Evt_HardwareFault);
		}
		else{
			while(DequeueEvent(&evt)){
				PWM_FSM_HandleEvent(evt);
			}
		}
		if(stateMachineFlag){
			stateMachineFlag = false;
			stateFunc[pwmState.currentState]();
		}
		if(sevenSegUpdateFlag){
			sevenSegUpdateFlag = 0;
			
			SevenSeg_BufferUpdate(&h7seg,buffer);
			SevenSeg_Update(&h7seg);
		}
		if(keyRead){
			keyRead = false;
			keyAct[pwmState.currentState]();
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//---------------------key functions start-----------------------------------------

//void pwmStartStopKeyCallback(PWM_State_t* pwm ){
//	
//}
//void screenChangeKeyCallback(void* P){
//	
//}
//---------------------key functions start-----------------------------------------
void PWM_Command_Key_init(volatile KeyPinConfig* key){
	key->GPIOPort = PWM_SS_GPIO_Port;
	key->GPIOPin = PWM_SS_Pin;
	key->state = KeyStateOnNone;
}

//timers NVIC callback-----------------------------------------
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){ //1 us timer
	if(htim->Instance == TIM2){
		if(fsm_tick_us++ >= stateTimingTable_us[pwmState.currentState]){
			fsm_tick_us = 0;
			stateMachineFlag = true;
		}
		if(keyCounter++ >= KEY_READ_PERIOD){
			keyCounter = 0;
			keyRead = true;
		}
	}
	
}
//timers NVIC callback
//

void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == ADC1){
		uint32_t injectedValue = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
		// calculate current value
	}
}
//
//---------------------------------------------------
//convert temperture values to sting for seven segment 


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
