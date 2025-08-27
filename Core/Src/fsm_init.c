#include "fsm_init.h"
#include "app_ctx.h"

volatile InitSub initMode = Init_DMA_Sampling;
const SM_Init_func initMachine[Init_END] = {
	[Init_connectPower]				=	init_connectPower,
	[Init_DMA_Sampling]				=	init_DMA_Sampling,
	[Init_DMA_Processing]			= init_DMA_Processing,
	[Init_safatyCheck]				= init_safatyCheck,
	[Init_calibratingCurrent]	= init_calibratingCurrent,
	[Init_Finishing]					= init_Finishing,
};
//-----------------------------------------------------
void init_connectPower			(struct AppContext* app){
	if(app->d_input.state == GPIO_PIN_RESET){
		
	}
}
void init_DMA_Sampling			(struct AppContext* app){
	if(app->adc.dmaSampleReady){
		initMode = Init_DMA_Processing;
		return;
	}
	DMA_Sampling(&app->adc);
}
void init_DMA_Processing		(struct AppContext* app){
	DMA_Processing(&app->adc);
	initMode = Init_safatyCheck;
}
void init_safatyCheck				(struct AppContext* app){
	safatyCheck(&app->adc);
	initMode = Init_calibratingCurrent;
}
void init_calibratingCurrent(struct AppContext* app){
	if(!app->adc.flags.calibrateDone){
		calibratingCurrent(&app->adc);
	}
	else{
		initMode = Init_Finishing;
	}
}
void init_Finishing					(struct AppContext* app){

}
//end of fsm_init.c 