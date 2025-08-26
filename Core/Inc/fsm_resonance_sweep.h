#ifndef __ADC_RESONANCE_SWEEP_H__
#define __ADC_RESONANCE_SWEEP_H__

#ifdef __cplusplus
extern "C" {
#endif
	
typedef enum{
	RS_sampling		= 0,
	RS_processing,
	RS_safatyCheck,
	RS_settingChanges,
	RS_Finishing,
	RS_END,
}ResonanceSweepSub;
typedef void (*SM_RS_func)(void);

extern volatile ResonanceSweepSub resonanceSweepMode;
extern const SM_RS_func resonanceSweepMachine[RS_END];

void resonanceSweep_sampling			(void);
void resonanceSweep_processing		(void);
void resonanceSweep_safatyCheck		(void);
void resonanceSweep_settingChanges(void);
void resonanceSweep_Finishing			(void);

#ifdef __cplusplus
}
#endif


#endif //__ADC_RESONANCE_SWEEP_H__