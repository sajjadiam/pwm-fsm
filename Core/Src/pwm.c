#include "app_ctx.h"
#include "tim.h"
#include "adc.h"
#include <string.h>


static PWM_State_t* s_ctx;
void PWM_AttachContext(PWM_State_t* ctx){
	s_ctx = ctx;
}

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
void reset_PWM_control_flags(PWM_State_t* s){
	s->flags.freqLock = PWM_FALSE;
	s->flags.freqRampDone = PWM_FALSE;
}
bool reset_PWM_control_variables(PWM_State_t* s){
	reset_PWM_control_flags(s);
	s->pwmTimer 				= &htim1;
	s->heatSinkTemp 		= 0.0f;
	s->voltage 					= 0.0f;
	s->current 					= 0.0f;
	
	s->currentFreq 			= 0;
	s->targetFreq 			= 0;
	s->currentDeadTime 	= 0;
	s->targetDeadTime 	= 0;
	s->targetPower			= 0;
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
bool set_PWM_control_variables(PWM_State_t* s){
	s->currentFreq = PWM_SOFT_START_START_FREQ;//10kHz
	s->targetFreq = PWM_SOFT_START_END_FREQ;//20kHz
	s->currentDeadTime = PWM_START_DEAD_TIME; //14us
	s->targetDeadTime = PWM_END_DEAD_TIME;//7us
	s->targetPower = PWM_SOFT_START_UPPER_LIMIT_POWER;
	s->flags.freqRampDone = PWM_FALSE;
	return true;
}
//-----------------------------------------
void Set_PWM_FrequencySmooth(PWM_State_t* s){
	uint32_t arr =  HAL_PWM_GetARR();
	uint32_t targetArr = (PWM_CLK_FREQ / (s->targetFreq * 2)) - 1;
  uint32_t cmp;
	if(s->flags.freqLock){
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
			s->flags.freqLock = 1;
		}
		if(!s->flags.freqLock){
			s->currentFreq = PWM_CLK_FREQ / ((arr + 1) * 2);
			cmp = (arr + 1) / 2;
			HAL_PWM_SetARR(arr);
			HAL_PWM_SetCompare(TIM_CHANNEL_1,cmp);
			HAL_PWM_SetCompare(TIM_CHANNEL_2,cmp);
			HAL_PWM_SetDeadTime(DT_ConvertNsToDTG(DT_FromFreq(s->currentFreq)));
		}
	}
}

//end of pwm.c