#ifndef __PWM_H__
#define __PWM_H__

//includes
#include <math.h>
#include "stm32f1xx_hal.h"
#include "pwm_hal.h"
#include "pwm_fsm.h" // ساختار کلي ماشين حالت و ايونت هايي که بايد انجام بشن 
#include "pwm_config.h"
#include "pwm_deadtime.h"
#include "adc_utils.h"
#include "time.h"
//defines

//typedefs
typedef uint16_t Counter;
typedef bool FLAG;
typedef enum{
	ERROR_CODE_None												= 0,
	
	ERROR_CODE_Evt_Start									= 10,
	ERROR_CODE_Evt_StartCommand,
	ERROR_CODE_Evt_StopCommand,
	ERROR_CODE_Evt_InitComplete,
	ERROR_CODE_Evt_SoftStartDone,
	ERROR_CODE_Evt_SoftStopDone,
	ERROR_CODE_Evt_ResonanceFound,
	ERROR_CODE_Evt_TargetPowerReached,
	ERROR_CODE_Evt_ResonanceLost,
	ERROR_CODE_Evt_TuningCommand,
	ERROR_CODE_Evt_TuningComplete,
	ERROR_CODE_Evt_OverPowerDetected,
	ERROR_CODE_Evt_OverTempDetected,
	ERROR_CODE_Evt_UnderVoltageDetected,
	ERROR_CODE_Evt_OverCurrentDetected,
	ERROR_CODE_Evt_ShortCircuitDetected,
	ERROR_CODE_Evt_HardwareFault,
	ERROR_CODE_Evt_ArcDetected,
	ERROR_CODE_Evt_GridFaultDetected,
	ERROR_CODE_Evt_FaultCleared,
	ERROR_CODE_PWM_PwmState								= 30,
	ERROR_CODE_PwmStateStandby,      			//بايد فکر کنم
	ERROR_CODE_PwmStateInit,             	
	ERROR_CODE_PwmStateSoftStart,        	
	ERROR_CODE_PwmStateResonanceSweep,   	
	ERROR_CODE_PwmStateRunning,          	
	ERROR_CODE_PwmStateRecovery,
	ERROR_CODE_PwmStateSoftStop, 
	ERROR_CODE_PwmStateHardStop, 
	ERROR_CODE_End,
}ERROR_CODE;
typedef struct{												// flags of pwm
	uint32_t freqRampDone 	: 1;
	uint32_t freqLock				: 1;
}PWM_Flags;


typedef struct{
	PWM_State_Machine currentState;	// current state 
	PWM_Flags					flags;
	float 						heatSinkTemp;							// from average of 2 adc chanel
	float 						voltage;									// from adc chanel
	float 						current;									// from adc chanel
	uint32_t 					currentFreq;						// from arr
	uint32_t 					targetFreq;						// clculate from 2 chanel of input capture
	uint16_t					targetPower;
	uint16_t					currentPower;
	uint8_t 					currentDeadTime;				// calculate from dtg register 
	uint8_t 					targetDeadTime;					// calculate from power & temperture & frequency
	ERROR_CODE 				errorCode;
	TIME							time;
}PWM_State_t;

extern PWM_State_t pwmState;
extern uint32_t fsm_tick_us;

extern volatile bool captureReadyCh3;
extern volatile bool captureReadyCh4;
extern volatile uint32_t CapturebuffCh4 [SAMPLE_NUM_MAX];
extern volatile uint32_t CapturebuffCh3 [SAMPLE_NUM_MAX];
//functions

void Set_PWM_FrequencySmooth(PWM_State_t* pwmState);									//Change the frequency to the target frequency slowly
void pwm_softStop(PWM_State_t* pwmState);

//------------------------------------

bool manual_PWM_Disable(void);         
bool manual_Timers_Reset(void);
void reset_PWM_control_flags(void);
bool reset_PWM_control_variables(void);
bool clear_fault_flags(void);
bool Enable_ProtectionInterrupts(void);
//----------------------------
void softStart_set_freq_ramp(void);
void softStart_sampling(void);
void softStart_tun_power(void);
void softStart_finishing(void);
void softStart_Processing(void);
//----------------------------------
bool manual_Timers_Enable(void);
bool set_PWM_frequency(uint16_t freq);
bool manual_PWM_Enable(void);
bool set_PWM_control_variables(PWM_State_t* pwmState);								//initial pwm structure
#endif//__PWM_H__