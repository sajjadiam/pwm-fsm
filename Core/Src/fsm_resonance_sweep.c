#include "fsm_resonance_sweep.h"
#include "adc_utils.h"
#include "input_capture_utils.h"

volatile ResonanceSweepSub resonanceSweepMode = RS_sampling;
const SM_RS_func resonanceSweepMachine[RS_END] = {
	[RS_sampling]				= resonanceSweep_sampling,
	[RS_processing]			= resonanceSweep_processing,
	[RS_safatyCheck]		= resonanceSweep_safatyCheck,
	[RS_settingChanges]	= resonanceSweep_settingChanges,
	[RS_Finishing]			= resonanceSweep_Finishing,
};

void resonanceSweep_sampling			(void){
	ADC_Context* ctx = &adcCtx;
	DMA_Sampling(ctx);
	INJECT_Sampling(ctx);
	if(captureHndler[IC_CH3].ready && captureHndler[IC_CH4].ready && ctx->dmaSampleReady && ctx->currentSampleReady){
		resonanceSweepMode = RS_processing;
	}
}
void resonanceSweep_processing		(void){
	
}
void resonanceSweep_safatyCheck		(void){
	
}
void resonanceSweep_settingChanges(void){
	
}
void resonanceSweep_Finishing			(void){
	
}
//end of fsm_resonance_sweep.h