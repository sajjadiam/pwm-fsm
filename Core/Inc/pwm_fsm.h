#ifndef __PWM_FSM_H__
#define __PWM_FSM_H__

#include <stdint.h>
#include <stdbool.h>

#define EVENT_QUEUE_SIZE 	10
#define TRANSITION_NUM		26

typedef enum{
	PwmStateStandby = 0,      	
	PwmStateInit,             	
	PwmStateSoftStart,        	
	PwmStateResonanceSweep,   	
	PwmStateRunning,          	
	PwmStateRecovery,
	PwmStateSoftStop, 
	PwmStateHardStop, 
	PwmStateEND
}PWM_State_Machine;

typedef void (*State_Machine_Func)(void);
/**
 * @enum PWM_Event_t
 * @brief Events that trigger transitions in the PWM FSM.
 */
typedef enum{
	Evt_None = 0,

	// ---- دستورات کنترلی ----
	Evt_StartCommand,
	Evt_StopCommand,

	// ---- اتمام مراحل ----
	Evt_InitComplete,
	Evt_SoftStartDone,
	Evt_SoftStopDone,
	Evt_ResonanceFound,

	// ---- کنترل حلقه توان و رزونانس ----
	Evt_TargetPowerReached,
	Evt_ResonanceLost,

	// ---- دستورات فرعی ----
	Evt_TuningCommand,
	Evt_TuningComplete,

	// ---- خطاهای نرم قابل بازیابی ----
	Evt_OverPowerDetected,
	Evt_OverTempDetected,
	Evt_UnderVoltageDetected,
	Evt_OverCurrentDetected,

	// ---- خطاهای سخت غیرقابل بازیابی ----
	Evt_ShortCircuitDetected,
	Evt_HardwareFault,
	Evt_ArcDetected,
	Evt_GridFaultDetected,
	
	// ---- سیستم ----
	Evt_FaultCleared,
	Evt_End
}PWM_Event_t;



typedef struct {
    PWM_Event_t queue[EVENT_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} EventQueue_t;

extern EventQueue_t eventQueue;
extern volatile bool stateMachineFlag;


typedef bool (*ActionFunc)(void);

typedef struct {
	PWM_State_Machine currentState; /**< Current state */
	PWM_Event_t event;              /**< Event triggering transition */
	ActionFunc action;              /**< Action function to execute */
	PWM_State_Machine nextState;   	/**< Next state */
}StateTransition_t;

void PWM_FSM_Init(void);			//ok

bool PWM_FSM_HandleEvent(PWM_Event_t event); //ok

PWM_State_Machine PWM_FSM_GetCurrentState(void); //ok
//-----------------------------------------------------
typedef enum{
	Init_MODE_DMA_Sampling			= 0,
	Init_MODE_DMA_Processing,
	Init_MODE_safatyCheck,
	Init_MODE_calibratingCurrent,
	Init_MODE_adcDisable,
	Init_MODE_initFinishing,
	Init_MODE_END,
}Init_MODE;




//----------------------------------------
bool Action_None(void);                // هیچ کاری انجام نمی‌ده (برای ترنزیشن‌های منطقی بدون عملیات)
bool Action_EnterInit(void);               // مقداردهی اولیه سخت‌افزار و پارامترها
bool Action_EnterSoftStart(void);     // شروع ramp-up اولیه PWM یا ولتاژ
bool Action_StartSweep(void);  
bool Action_EnterRunning(void);
bool Action_RecoveryMode(void);
bool Action_Tune(void);        
bool Action_SoftStop(void);    
bool Action_FailSoftStop(void);
bool Action_Shutdown(void);    
bool Action_EnterStandby(void);
bool Action_SoftRestart(void); 
bool Action_ResetSystem(void); 
//-------------------------------
void stateStandby(void);
void stateInit(void);
void stateSoftStart(void);
void stateResonanceSweep(void);
void stateRunning(void);
void stateRecovery(void);
void stateSoftStop(void);
void stateHardStop(void);
//----------------------------------------------
void reset_fsm_control_flags(void);
extern const StateTransition_t pwmFsmTable[TRANSITION_NUM];
extern const uint32_t stateTimingTable_us[PwmStateEND];
//-----------------------------------------------------------
bool EnqueueEvent(PWM_Event_t evt);
bool DequeueEvent(PWM_Event_t* evt);
void EventQueue_Clear(void); 
//---------------------------------------------
typedef void (*keyAction)(void);
extern const keyAction keyAct[PwmStateEND];
void stateStandby_keyCallback(void);
void stateInit_keyCallback(void);
void stateSoftStart_keyCallback(void);
void stateResonanceSweep_keyCallback(void);
void stateRunning_keyCallback(void);
void stateRecovery_keyCallback(void);
void stateSoftStop_keyCallback(void);
void stateHardStop_keyCallback(void);

#endif // __PWM_FSM_H__
