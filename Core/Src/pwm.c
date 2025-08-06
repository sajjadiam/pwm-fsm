#include "pwm.h"
#include "tim.h"
uint32_t fsm_tick_us = 0;
PWM_State_t pwmState;

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
	pwmState.flags.ssFirstIn = PWM_FALSE;
	pwmState.flags.softStarted = PWM_FALSE;
	pwmState.flags.freqLock = PWM_FALSE;
	pwmState.flags.dtLock = PWM_FALSE;
	pwmState.flags.overPower = PWM_FALSE;
	pwmState.flags.softStop = PWM_FALSE;
	pwmState.flags.stateChange = PWM_FALSE;
	pwmState.flags.initDone = 1;
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
	return true;
}
//-----------------------------------------
void pwm_softStart(PWM_State_t* pwmState){
	uint32_t arr;
	uint32_t cmp;
	if(pwmState->flags.softStarted){
		return;
	}
	else{
		if(pwmState->flags.ssFirstIn){
			pwmState->flags.ssFirstIn = PWM_FALSE;
//			Start_PWM_Safe(pwmState);
		}
		else{
			arr = HAL_PWM_GetARR();
		}
		arr -= PWM_SOFT_START_STEP;
		if(arr < PWM_END_ARR){
			arr = PWM_END_ARR;
		}
		pwmState->currentFreq = PWM_CLK_FREQ / ((arr + 1) * 2);
		cmp = (arr + 1) / 2;
		HAL_PWM_SetARR(arr);
		HAL_PWM_SetCompare(TIM_CHANNEL_1,cmp);
		HAL_PWM_SetCompare(TIM_CHANNEL_2,cmp);

		if (pwmState->currentDeadTime > pwmState->targetDeadTime) {
			pwmState->currentDeadTime--;
		}
		HAL_PWM_SetDeadTime(pwmState->currentDeadTime);

		if(arr == PWM_END_ARR && pwmState->currentDeadTime == PWM_END_DEAD_TIME){
			pwmState->flags.softStarted = PWM_TRUE;
			//setPwmState(pwmState,PwmStateRunning);
		}
	}
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
//--------------------------
void pwm_softStop(PWM_State_t* pwmState){

}
