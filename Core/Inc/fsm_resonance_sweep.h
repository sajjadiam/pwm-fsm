#ifndef __ADC_RESONANCE_SWEEP_H__
#define __ADC_RESONANCE_SWEEP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

struct AppContext;
typedef enum{
	RS_sampling		= 0,
	RS_processing,
	RS_safatyCheck,
	RS_settingChanges,
	RS_Finishing,
	RS_END,
}ResonanceSweepSub;
typedef void (*SM_RS_func)(struct AppContext* app);

extern volatile ResonanceSweepSub resonanceSweepMode;
extern const SM_RS_func resonanceSweepMachine[RS_END];

void resonanceSweep_sampling			(struct AppContext* app);
void resonanceSweep_processing		(struct AppContext* app);
void resonanceSweep_safatyCheck		(struct AppContext* app);
void resonanceSweep_settingChanges(struct AppContext* app);
void resonanceSweep_Finishing			(struct AppContext* app);
bool Calibrate_PhaseOffset(struct AppContext* app, uint16_t N);
#ifdef __cplusplus
}
#endif


#endif //__ADC_RESONANCE_SWEEP_H__