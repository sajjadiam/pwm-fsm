#include "pwm.h"
#include "tim.h"
#include "adc.h"
#include <string.h>

uint32_t fsm_tick_us = 0;
static uint16_t softStart_arrCounter = 0;
PWM_State_t pwmState;
IC_Handler captureHndler[IC_END];

bool manual_PWM_Disable(void){
	HAL_PWM_Stop(); // متوقف کردن خروجی PWM

	// اگر نیاز داری تایمر هم کلاً متوقف بشه، این خط هم اضافه کن:
	// __HAL_TIM_DISABLE(&htim1);

	return true; // فرض بر اینه که خطایی نداره، ولی می‌تونی بعداً چک هم اضافه کنی
} 
bool manual_Timers_Reset(void){
	HAL_PWM_Stop();
	HAL_PWM_Reset_Counter();
	HAL_PWM_SetCompare(TIM_CHANNEL_1,0);
	HAL_PWM_SetCompare(TIM_CHANNEL_2,0);
	HAL_PWM_TIMER_Disable();
	return true;
}
void reset_PWM_control_flags(void){
	pwmState.flags.freqLock = PWM_FALSE;
	pwmState.flags.freqRampDone = PWM_FALSE;
}
bool reset_PWM_control_variables(void){
	
	reset_PWM_control_flags();
	
	pwmState.heatSinkTemp 		= 0.0f;
	pwmState.voltage 					= 0.0f;
	pwmState.current 					= 0.0f;
	
	pwmState.currentFreq 			= 0;
	pwmState.targetFreq 			= 0;
	pwmState.currentDeadTime 	= 0;
	pwmState.targetDeadTime 	= 0;
	pwmState.targetPower			= 0;
	return true;
}
bool clear_fault_flags(void){
	
	return true;
}
bool Enable_ProtectionInterrupts(void){
	// 1) فعال‌سازی وقفه‌ی Break تایمر TIM1
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_BREAK);
	HAL_NVIC_SetPriority(TIM1_BRK_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_BRK_IRQn);

	// 2) (اختیاری) اگر از ADC watchdog برای over-temperature یا over-voltage استفاده می‌کنی:
	// __HAL_ADC_ENABLE_IT(&hadc1, ADC_IT_AWD);

	// 3) (اختیاری) اگر یک پین EXTI به‌عنوان Fault ورودی داری:
	// HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
	// HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	return true;
}
//--------------------------------------
bool manual_Timers_Enable(void){
	HAL_PWM_TIMER_Enable();
	return true;
}
bool set_PWM_frequency(uint16_t freq){
	HAL_PWM_Init(freq);
	return true;
}
bool manual_PWM_Enable(void){
	HAL_PWM_Start();
	return true;
}
bool set_PWM_control_variables(PWM_State_t* pwmState){
	pwmState->currentFreq = PWM_SOFT_START_START_FREQ;//10kHz
	pwmState->targetFreq = PWM_SOFT_START_END_FREQ;//20kHz
	pwmState->currentDeadTime = PWM_START_DEAD_TIME; //14us
	pwmState->targetDeadTime = PWM_END_DEAD_TIME;//7us
	pwmState->targetPower = PWM_SOFT_START_UPPER_LIMIT_POWER;
	pwmState->flags.freqRampDone = PWM_FALSE;
	return true;
}
//-----------------------------------------
void softStart_set_freq_ramp(void){
	uint32_t arr = HAL_PWM_GetARR();
	if(arr <= PWM_END_ARR && pwmState.currentDeadTime <= PWM_END_DEAD_TIME){
		if(!manual_ADC_Enable()){
			//error code
			return;
		}
		softStartMode = SOFT_START_MODE_sampling;
		return;
	}
	//setting arr
	arr -= PWM_SOFT_START_STEP; // step => 6 
	if(arr < PWM_END_ARR){
		arr = PWM_END_ARR;
	}
	pwmState.currentFreq = PWM_CLK_FREQ / ((arr + 1) * 2);
	uint32_t cmp = (arr + 1) / 2;

	HAL_PWM_SetARR(arr);
	HAL_PWM_SetCompare(TIM_CHANNEL_1,cmp);
	HAL_PWM_SetCompare(TIM_CHANNEL_2,cmp);
	//setting dead time
	if(softStart_arrCounter >= 56){
		softStart_arrCounter -= 56;
		if (pwmState.currentDeadTime > pwmState.targetDeadTime) {
			pwmState.currentDeadTime--;
		}
	}
	HAL_PWM_SetDeadTime(pwmState.currentDeadTime);
	softStart_arrCounter += PWM_SOFT_START_STEP;
	return ;
}
void softStart_sampling(void){
	if(dmaSampleReady == false){
		DMA_GET_SAMPLE();
	}
	if(currentSampleReady == false){
		INJECT_GET_SAMPLE();
	}
	else if(currentSampleReady == true && dmaSampleReady == true){
		softStartMode = SOFT_START_MODE_processing;
		currentSampleReady = false;
		dmaSampleReady = false;
	}
}
void softStart_Processing(void){
	uint32_t sampleSum = 0;
	for(uint16_t i = 0;i < SampleNum[pwmState.currentState];i++){
		sampleSum += voltageSample[i];
	}
	dmaSampleMean[ADC_IDX_VBUS] = (uint16_t)((sampleSum / SampleNum[pwmState.currentState]) + 0.5f);
	//--
	sampleSum = 0;
	for(uint16_t i = 0;i < SampleNum[pwmState.currentState];i++){
		sampleSum += temp1Sample[i];
	}
	dmaSampleMean[ADC_IDX_TEMP_CH1] = (uint16_t)((sampleSum / SampleNum[pwmState.currentState]) + 0.5f);
	//--
	sampleSum = 0;
	for(uint16_t i = 0;i < SampleNum[pwmState.currentState];i++){
		sampleSum += temp2Sample[i];
	}
	dmaSampleMean[ADC_IDX_TEMP_CH2] = (uint16_t)((sampleSum / SampleNum[pwmState.currentState]) + 0.5f);
	//--
	for(uint16_t i = 0;i < SampleNum[pwmState.currentState];i++){
		sampleSum += currentSample[i];
	}
	currentSmpleMean = (uint16_t)((sampleSum / SampleNum[pwmState.currentState]) + 0.5f);
	softStartMode = SOFT_START_MODE_tunPower;
}
void softStart_tun_power(void){
	pwmState.voltage = ADC_to_voltage(dmaSampleMean[ADC_IDX_VBUS]);   // ولتاژ باس
	pwmState.current = ADC_to_current(currentSmpleMean); 							// جریان
	pwmState.currentPower = pwmState.voltage * pwmState.current;      // توان لحظه‌ای
	if(pwmState.currentPower > PWM_SOFT_START_LOWER_LIMIT_POWER && pwmState.currentPower < PWM_SOFT_START_UPPER_LIMIT_POWER){
		softStartMode = SOFT_START_MODE_Finishing;
		return;
	}
	else if(pwmState.currentPower < PWM_SOFT_START_LOWER_LIMIT_POWER){
		pwmState.currentDeadTime--;
	}
	else if(pwmState.currentPower > PWM_SOFT_START_UPPER_LIMIT_POWER){
		pwmState.currentDeadTime++;
	}
	HAL_PWM_SetDeadTime(pwmState.currentDeadTime);
	softStartMode = SOFT_START_MODE_sampling;
}
void softStart_finishing(void){
	EnqueueEvent(Evt_SoftStartDone);
}
void Set_PWM_FrequencySmooth(PWM_State_t* pwmState){
	uint32_t arr =  HAL_PWM_GetARR();
	uint32_t targetArr = (PWM_CLK_FREQ / (pwmState->targetFreq * 2)) - 1;
  uint32_t cmp;
	if(pwmState->flags.freqLock){
		return;
	}
	else{
		if(arr < targetArr){
			arr += PWM_SOFT_CHANGE_STEP;
		}
		else if(arr > targetArr){
			arr -= PWM_SOFT_CHANGE_STEP;
		}
		else{
			pwmState->flags.freqLock = 1;
		}
		if(!pwmState->flags.freqLock){
			pwmState->currentFreq = PWM_CLK_FREQ / ((arr + 1) * 2);
			cmp = (arr + 1) / 2;
			HAL_PWM_SetARR(arr);
			HAL_PWM_SetCompare(TIM_CHANNEL_1,cmp);
			HAL_PWM_SetCompare(TIM_CHANNEL_2,cmp);
			HAL_PWM_SetDeadTime(DT_ConvertNsToDTG(DT_FromFreq(pwmState->currentFreq)));
		}
	}
}
//input capture
void captureUnitInit(IC_Handler *cu ,TIM_HandleTypeDef *htim ,uint32_t channel){
	cu->htim = htim;
	cu->ch = channel;
	cu->armed = 1;
	cu->ready = 0;
	cu->count = 0;
	cu->avg = 0;
	memset(cu->buff,0,sizeof cu->buff);
}
void IC_Init(void){
	captureUnitInit(&captureHndler[IC_CH3],&htim1,TIM_CHANNEL_3);
	captureUnitInit(&captureHndler[IC_CH4],&htim1,TIM_CHANNEL_4);
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM1){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3){
			IC_Handler* cu = &captureHndler[IC_CH3];
			if (cu->armed && cu->count < SAMPLE_NUM) {
				cu->buff[cu->count++] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
				if (cu->count >= SAMPLE_NUM){ 
					cu->armed = 0;
					cu->ready = 1; 
					cu->count = 0;
				}
			}
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4){
			IC_Handler* cu = &captureHndler[IC_CH4];
			if (cu->armed && cu->count < SAMPLE_NUM) {
				cu->buff[cu->count++] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
				if (cu->count >= SAMPLE_NUM){ 
					cu->armed = 0;
					cu->ready = 1; 
					cu->count = 0;
				}
			}
		}
	}
}
void IC_processSample(IC_Handler *cu ,uint16_t len){
	uint8_t i;
	const uint8_t sampleNumber = SAMPLE_NUM - 1;
	while(len-- > 0){
		uint32_t sampleSum = 0;
		for(i = 0;i < sampleNumber;i++){
			sampleSum += ((cu->buff[i + 1] > cu->buff[i]) ? (cu->buff[i + 1] - cu->buff[i])
				: ((cu->htim->Init.Period + 1u) - cu->buff[i] + cu->buff[i + 1]));
		}
		cu->avg = sampleSum / sampleNumber;
		cu->ready = 0;
		cu++;
	}
}
//--------------------------

//end of pwm.c