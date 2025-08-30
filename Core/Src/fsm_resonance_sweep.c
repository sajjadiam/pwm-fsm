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
	if(app->ic.ch[IC_CH_I].ready && app->ic.ch[IC_CH_V].ready && ctx->dmaSampleReady && ctx->currentSampleReady){
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
bool Calibrate_PhaseOffset(struct AppContext* app, uint16_t N){
	uint32_t acc_mdeg = 0;
	uint16_t k = 0;

	while(k < N){
			// همزمان هر دو کانال را آرم کن
		IC_getSample(&app->ic, 2);

		// منتظر پر شدن بافر هر دو کانال
		while(!(app->ic.ch[IC_CH_I].ready && app->ic.ch[IC_CH_V].ready)){
				// بهتر است یک تایم‌اوت هم داشته باشی تا گیر نکند
		}

		// میانگین دوره و آخرین لبه معتبر را محاسبه کن
		IC_processSample(&app->ic, 2);

		// محاسبه فاز لحظه‌ای
		float phase = calc_phase_deg(&app->ic);

		// جمع با مقیاس هزارم درجه برای کاهش خطای float در حلقه
		acc_mdeg += (uint32_t)lroundf(phase * 1000.0f);
		k++;
	}
	
	float mean_deg = ((float)acc_mdeg / (float)k) / 1000.0f;
	app->pwm.phase_offset_vc_deg = mean_deg; // در state نگه دار
	return true;
}
//end of fsm_resonance_sweep.h