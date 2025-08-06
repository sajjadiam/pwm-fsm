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
	uint16_t initDone   	:1;
	uint16_t ssFirstIn   	:1;
	uint16_t softStarted 	:1;
	uint16_t freqLock    	:1;
	uint16_t dtLock     	:1;
	uint16_t overPower   	:1;
	uint16_t softStop    	:1;
	uint16_t Stopcomplete :1;
	uint16_t stateChange 	:1;
	uint16_t faultActive  :1;
	uint16_t pwmEnabled   :1;
	uint16_t manualStop   :1;
}PWM_Flags;


typedef struct{
	PWM_State_Machine currentState;	// current state 
	PWM_Flags					flags;
	float 						heatSinkTemp;							// from average of 2 adc chanel
	float 						voltage;									// from adc chanel
	float 						current;									// from adc chanel
	uint32_t 					currentFreq;						// from arr
	uint32_t 					targetFreq;						// clculate from 2 chanel of input capture
	uint8_t 					currentDeadTime;				// calculate from dtg register 
	uint8_t 					targetDeadTime;					// calculate from power & temperture & frequency
}PWM_State_t;

extern PWM_State_t pwmState;
extern uint32_t fsm_tick_us;
extern ERROR_CODE errorCode;
//functions
void pwm_init(PWM_State_t* pwmState);																	//initial pwm structure
void Start_PWM_Safe(PWM_State_t* pwmState);														//initial timer for safe starting
void pwm_softStart(PWM_State_t* pwmState);														//soft start function
void Set_PWM_FrequencySmooth(PWM_State_t* pwmState);									//Change the frequency to the target frequency slowly
void pwm_softStop(PWM_State_t* pwmState);

//------------------------------------

bool manual_PWM_Disable(void);         
bool manual_Timers_Reset(void);
void reset_PWM_control_flags(void);
bool reset_PWM_control_variables(void);
bool clear_fault_flags(void);
bool Enable_ProtectionInterrupts(void);
//----------------------------------
bool manual_Timers_Enable(void);
bool set_PWM_frequency(uint16_t freq);
bool manual_PWM_Enable(void);
#endif//__PWM_H__