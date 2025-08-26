#include "fsm_init.h"
#include "app_ctx.h"

volatile InitSub initMode = Init_DMA_Sampling;
const SM_Init_func initMachine[Init_END] = {
	[Init_DMA_Sampling]				=	init_DMA_Sampling,
	[Init_DMA_Processing]			= init_DMA_Processing,
	[Init_safatyCheck]				= init_safatyCheck,
	[Init_calibratingCurrent]	= init_calibratingCurrent,
	[Init_adcDisable]					= init_adcDisable,
//	[Init_Finishing]					= init_Finishing,
};

void init_DMA_Sampling			(struct AppContext* app){
	if(DMA_Sampling(&app->adc) == false){
		return;
	}
	initMode = Init_DMA_Processing;
}
void init_DMA_Processing		(struct AppContext* app){
	if(DMA_Processing(&app->adc) == false){
		return;
	}
	initMode = Init_safatyCheck;
}
void init_safatyCheck				(struct AppContext* app){
	if(safatyCheck(&app->adc) == false){
		return;
	}
	initMode = Init_calibratingCurrent;
}
void init_calibratingCurrent(struct AppContext* app){
	if(calibratingCurrent(&app->adc) == false){
		return;
	}
	initMode = Init_adcDisable;
}
void init_adcDisable				(struct AppContext* app){
	if(adcDisable(&app->adc) == false){
		return;
	}
}
//void init_Finishing					(struct AppContext* app);
//end of fsm_init.c 