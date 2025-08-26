#ifndef __PWM_FSM_H__
#define __PWM_FSM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "fsm_events.h"

#define TRANSITION_NUM			27U
#define SAMPLE_NUM_RUNNING	5U
	
struct AppContext; // forward declaration

/**
 * @enum PWM_Event_t
 * @brief Events that trigger transitions in the PWM FSM.
 */
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

typedef bool (*ActionFunc)(struct AppContext* app);

typedef struct {
	PWM_State_Machine currentState; /**< Current state */
	PWM_Event_t event;              /**< Event triggering transition */
	ActionFunc action;              /**< Action function to execute */
	PWM_State_Machine nextState;   	/**< Next state */
}StateTransition_t;

void PWM_FSM_Init(struct AppContext* app);			//ok
bool PWM_FSM_HandleEvent(struct AppContext* app, PWM_Event_t event); //ok

PWM_State_Machine PWM_FSM_GetCurrentState(struct AppContext* app); //ok
//----------------------------------------
bool Action_None(struct AppContext* app);                // هیچ کاری انجام نمی‌ده (برای ترنزیشن‌های منطقی بدون عملیات)
bool Action_EnterInit(struct AppContext* app);               // مقداردهی اولیه سخت‌افزار و پارامترها
bool Action_EnterSoftStart(struct AppContext* app);     // شروع ramp-up اولیه PWM یا ولتاژ
bool Action_StartSweep(struct AppContext* app);  
bool Action_EnterRunning(struct AppContext* app);
bool Action_RecoveryMode(struct AppContext* app);
bool Action_Tune(struct AppContext* app);        
bool Action_SoftStop(struct AppContext* app);    
bool Action_FailSoftStop(struct AppContext* app);
bool Action_Shutdown(struct AppContext* app);    
bool Action_EnterStandby(struct AppContext* app);
bool Action_SoftRestart(struct AppContext* app); 
bool Action_ResetSystem(struct AppContext* app); 
//-------------------------------
typedef void (*State_Machine_Func)(struct AppContext* app);
void stateStandby(struct AppContext* app);
void stateInit(struct AppContext* app);
void stateSoftStart(struct AppContext* app);
void stateResonanceSweep(struct AppContext* app);
void stateRunning(struct AppContext* app);
void stateRecovery(struct AppContext* app);
void stateSoftStop(struct AppContext* app);
void stateHardStop(struct AppContext* app);
//----------------------------------------------
extern const StateTransition_t pwmFsmTable[TRANSITION_NUM];
extern const uint32_t stateTimingTable_us[8];
//-----------------------------------------------------------
bool EnqueueEvent(struct AppContext* app ,PWM_Event_t evt);
bool DequeueEvent(struct AppContext* app ,PWM_Event_t* evt);
//---------------------------------------------
typedef void (*keyAction)(struct AppContext* app);
extern const keyAction keyAct[8];
void stateStandby_keyCallback				(struct AppContext* app);
void stateInit_keyCallback					(struct AppContext* app);
void stateSoftStart_keyCallback			(struct AppContext* app);
void stateResonanceSweep_keyCallback(struct AppContext* app);
void stateRunning_keyCallback				(struct AppContext* app);
void stateRecovery_keyCallback			(struct AppContext* app);
void stateSoftStop_keyCallback			(struct AppContext* app);
void stateHardStop_keyCallback			(struct AppContext* app);
#ifdef __cplusplus
}
#endif
#endif // __PWM_FSM_H__
