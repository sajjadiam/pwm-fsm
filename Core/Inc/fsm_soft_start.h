#ifndef __FSM_SOFT_START_H__
#define __FSM_SOFT_START_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	SS_setFrequencyRamp		= 0,
	SS_sampling,
	SS_processing,
	SS_safatyCheck,
	SS_tunPower,
	SS_Finishing,
	SS_END,
}SoftStartSub;
typedef void (*SM_SS_func)(void);

void ss_set_freq_ramp	(void);
void ss_sampling			(void);
void ss_Processing		(void);
void ss_safatyCheck		(void);
void ss_tun_power			(void);
void ss_finishing			(void);

extern volatile SoftStartSub softStartMode;
extern const 		SM_SS_func softStartMachine[SS_END];

#ifdef __cplusplus
}
#endif

#endif //__FSM_SOFT_START_H__