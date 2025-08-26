#include "fsm_soft_start.h"
#include "pwm.h"
#include "adc_utils.h"

static uint16_t softStart_arrCounter = 0;
const SM_SS_func softStartMachine[SS_END] = {
	[SS_setFrequencyRamp]	= ss_set_freq_ramp,
	[SS_sampling]					= ss_sampling,
	[SS_processing]				= ss_Processing,
	[SS_safatyCheck]			= ss_safatyCheck	,
	[SS_tunPower]					= ss_tun_power,
	[SS_Finishing]				= ss_finishing,
};
volatile SoftStartSub softStartMode = SS_setFrequencyRamp;

void ss_set_freq_ramp	(void){
	uint32_t arr = HAL_PWM_GetARR();
	if(arr <= PWM_END_ARR && pwmState.currentDeadTime <= PWM_END_DEAD_TIME){
		if(!manual_ADC_Enable(&adcCtx)){
			//error code
			return;
		}
		softStartMode = SS_sampling;
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
	return;
}
void ss_sampling			(void){
	ADC_Context* ctx = &adcCtx;
	DMA_Sampling(ctx);
	INJECT_Sampling(ctx);
	if(ctx->dmaSampleReady && ctx->currentSampleReady){
		softStartMode = SS_processing;
	}
}
void ss_Processing		(void){
	ADC_Processing(&adcCtx);
	softStartMode = SS_safatyCheck;
}
void ss_safatyCheck		(void){
	if(safatyCheck(&adcCtx) == false){
		return;
	}
	softStartMode = SS_tunPower;
}
void ss_tun_power			(void){
	ADC_Context* ctx = &adcCtx;
	pwmState.voltage = ADC_to_voltage(ctx->sampleMean[ADC_Channel_Voltage]);   // ولتاژ باس
	pwmState.current = ADC_to_current(ctx->sampleMean[ADC_Channel_Curent]); 							// جریان
	pwmState.currentPower = pwmState.voltage * pwmState.current;      // توان لحظه‌ای
	if(pwmState.currentPower > PWM_SOFT_START_LOWER_LIMIT_POWER && pwmState.currentPower < PWM_SOFT_START_UPPER_LIMIT_POWER){
		softStartMode = SS_Finishing;
		return;
	}
	else if(pwmState.currentPower < PWM_SOFT_START_LOWER_LIMIT_POWER){
		pwmState.currentDeadTime--;
	}
	else if(pwmState.currentPower > PWM_SOFT_START_UPPER_LIMIT_POWER){
		pwmState.currentDeadTime++;
	}
	HAL_PWM_SetDeadTime(pwmState.currentDeadTime);
	softStartMode = SS_sampling;
}
void ss_finishing			(void){
	
}