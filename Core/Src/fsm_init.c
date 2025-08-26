#include "fsm_init.h"
#include "adc_utils.h"

volatile InitSub initMode = Init_DMA_Sampling;
const SM_Init_func initMachine[Init_END] = {
	[Init_DMA_Sampling]				=	init_DMA_Sampling,
	[Init_DMA_Processing]			= init_DMA_Processing,
	[Init_safatyCheck]				= init_safatyCheck,
	[Init_calibratingCurrent]	= init_calibratingCurrent,
	[Init_adcDisable]					= init_adcDisable,
//	[Init_Finishing]					= init_Finishing,
};

void init_DMA_Sampling			(void){
	if(DMA_Sampling(&adcCtx) == false){
		return;
	}
	initMode = Init_DMA_Processing;
}
void init_DMA_Processing		(void){
	if(DMA_Processing(&adcCtx) == false){
		return;
	}
	initMode = Init_safatyCheck;
}
void init_safatyCheck				(void){
	if(safatyCheck(&adcCtx) == false){
		return;
	}
	initMode = Init_calibratingCurrent;
}
void init_calibratingCurrent(void){
	if(calibratingCurrent(&adcCtx) == false){
		return;
	}
	initMode = Init_adcDisable;
}
void init_adcDisable				(void){
	if(adcDisable(&adcCtx) == false){
		return;
	}
}
//void init_Finishing					(void);
//end of fsm_init.c 