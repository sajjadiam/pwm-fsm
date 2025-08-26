#include "fsm_soft_start.h"
#include "app_ctx.h"

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

void ss_set_freq_ramp	(struct AppContext* app){
	uint32_t arr = HAL_PWM_GetARR();
	if(arr <= PWM_END_ARR && app->pwm.currentDeadTime <= PWM_END_DEAD_TIME){
		if(!manual_ADC_Enable(&app->adc)){
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
	app->pwm.currentFreq = PWM_CLK_FREQ / ((arr + 1) * 2);
	uint32_t cmp = (arr + 1) / 2;

	HAL_PWM_SetARR(arr);
	HAL_PWM_SetCompare(TIM_CHANNEL_1,cmp);
	HAL_PWM_SetCompare(TIM_CHANNEL_2,cmp);
	//setting dead time
	if(softStart_arrCounter >= 56){
		softStart_arrCounter -= 56;
		if (app->pwm.currentDeadTime > app->pwm.targetDeadTime) {
			app->pwm.currentDeadTime--;
		}
	}
	HAL_PWM_SetDeadTime(app->pwm.currentDeadTime);
	softStart_arrCounter += PWM_SOFT_START_STEP;
	return;
}
void ss_sampling			(struct AppContext* app){
	ADC_Context* ctx = &app->adc;
	DMA_Sampling(ctx);
	INJECT_Sampling(ctx);
	if(ctx->dmaSampleReady && ctx->currentSampleReady){
		softStartMode = SS_processing;
	}
}
void ss_Processing		(struct AppContext* app){
	ADC_Processing(&app->adc);
	softStartMode = SS_safatyCheck;
}
void ss_safatyCheck		(struct AppContext* app){
	
	if(safatyCheck(&app->adc) == false){
		return;
	}
	softStartMode = SS_tunPower;
}
void ss_tun_power			(struct AppContext* app){
	ADC_Context* ctx = &app->adc;
	app->pwm.voltage = ADC_to_voltage(ctx->sampleMean[ADC_Channel_Voltage]);   // ولتاژ باس
	app->pwm.current = ADC_to_current(ctx->sampleMean[ADC_Channel_Curent]); 							// جریان
	app->pwm.currentPower = app->pwm.voltage * app->pwm.current;      // توان لحظه‌ای
	if(app->pwm.currentPower > PWM_SOFT_START_LOWER_LIMIT_POWER && app->pwm.currentPower < PWM_SOFT_START_UPPER_LIMIT_POWER){
		softStartMode = SS_Finishing;
		return;
	}
	else if(app->pwm.currentPower < PWM_SOFT_START_LOWER_LIMIT_POWER){
		app->pwm.currentDeadTime--;
	}
	else if(app->pwm.currentPower > PWM_SOFT_START_UPPER_LIMIT_POWER){
		app->pwm.currentDeadTime++;
	}
	HAL_PWM_SetDeadTime(app->pwm.currentDeadTime);
	softStartMode = SS_sampling;
}
void ss_finishing			(struct AppContext* app){
	
}