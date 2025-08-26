#include "fsm_resonance_sweep.h"
#include "app_ctx.h"



volatile ResonanceSweepSub resonanceSweepMode = RS_sampling;
const SM_RS_func resonanceSweepMachine[RS_END] = {
	[RS_sampling]				= resonanceSweep_sampling,
	[RS_processing]			= resonanceSweep_processing,
	[RS_safatyCheck]		= resonanceSweep_safatyCheck,
	[RS_settingChanges]	= resonanceSweep_settingChanges,
	[RS_Finishing]			= resonanceSweep_Finishing,
};

void resonanceSweep_sampling			(struct AppContext* app){
	ADC_Context* ctx = &app->adc;
	DMA_Sampling(ctx);
	INJECT_Sampling(ctx);
	if(app->ic.ch[IC_CH3].ready && app->ic.ch[IC_CH4].ready && ctx->dmaSampleReady && ctx->currentSampleReady){
		resonanceSweepMode = RS_processing;
	}
}
void resonanceSweep_processing		(struct AppContext* app){
	
}
void resonanceSweep_safatyCheck		(struct AppContext* app){
	
}
void resonanceSweep_settingChanges(struct AppContext* app){
	
}
void resonanceSweep_Finishing			(struct AppContext* app){
	
}
//end of fsm_resonance_sweep.h