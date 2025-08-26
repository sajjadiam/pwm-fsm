#ifndef __FSM_SOFT_START_H__
#define __FSM_SOFT_START_H__

#ifdef __cplusplus
extern "C" {
#endif

struct AppContext; // forward declaration

typedef enum{
	SS_setFrequencyRamp		= 0,
	SS_sampling,
	SS_processing,
	SS_safatyCheck,
	SS_tunPower,
	SS_Finishing,
	SS_END,
}SoftStartSub;
typedef void (*SM_SS_func)(struct AppContext* app);

void ss_set_freq_ramp	(struct AppContext* app);
void ss_sampling			(struct AppContext* app);
void ss_Processing		(struct AppContext* app);
void ss_safatyCheck		(struct AppContext* app);
void ss_tun_power			(struct AppContext* app);
void ss_finishing			(struct AppContext* app);

extern volatile SoftStartSub softStartMode;
extern const 		SM_SS_func softStartMachine[SS_END];

#ifdef __cplusplus
}
#endif

#endif //__FSM_SOFT_START_H__