#include "pwm_fsm.h"
#include "app_ctx.h"          // حالا اینجا تعریف کامل AppContext را داریم
#include "fsm_init.h"
#include "fsm_soft_start.h"
#include "fsm_resonance_sweep.h"


//static AppContext app_ctx;
ERROR_CODE errorCode = ERROR_CODE_None;



const StateTransition_t transitions[TRANSITION_NUM] ={
	// ====== STANDBY ======
	{PwmStateStandby, 				Evt_StartCommand,      			Action_EnterInit,         PwmStateInit						},	// 1
	// ====== INIT ======
	{PwmStateInit,    				Evt_InitComplete,      			Action_EnterSoftStart ,   PwmStateSoftStart				},	// 2
	{PwmStateInit, 						Evt_StopCommand,           	Action_EnterStandby,      PwmStateStandby					},	// 3
	// ====== SOFT START =====
	{PwmStateSoftStart, 			Evt_SoftStartDone,   				Action_StartSweep,        PwmStateResonanceSweep	},	// 4
	{PwmStateSoftStart, 			Evt_StopCommand,           	Action_SoftStop,       		PwmStateSoftStop				},	// 5
	{PwmStateSoftStart, 			Evt_HardwareFault,  				Action_Shutdown,      		PwmStateHardStop				},	// 6
	// ====== RESONANCE SWEEP ======
	{PwmStateResonanceSweep, 	Evt_ResonanceFound, 				Action_EnterRunning,  		PwmStateRunning					},	// 7
	{PwmStateResonanceSweep, 	Evt_OverPowerDetected, 			Action_EnterRunning,  		PwmStateRunning					},	// 8
	{PwmStateResonanceSweep, 	Evt_ResonanceLost,  				Action_FailSoftStop,  		PwmStateSoftStop				},	// 9
	{PwmStateResonanceSweep, 	Evt_StopCommand,  					Action_FailSoftStop,  		PwmStateSoftStop				},	// 10
	{PwmStateResonanceSweep, 	Evt_HardwareFault,  				Action_Shutdown,      		PwmStateHardStop				},	// 11
	// ====== RUNNING ======
	{PwmStateRunning, 				Evt_TargetPowerReached,    	Action_None,           		PwmStateRunning					},	// 12
	{PwmStateRunning, 				Evt_StopCommand,           	Action_SoftStop,       		PwmStateSoftStop				},	// 13
	{PwmStateRunning, 				Evt_ResonanceLost,         	Action_RecoveryMode,   		PwmStateRecovery				},	// 14
	{PwmStateRunning, 				Evt_OverPowerDetected,     	Action_RecoveryMode,   		PwmStateRecovery				},	// 15
	{PwmStateRunning, 				Evt_OverTempDetected,      	Action_RecoveryMode,   		PwmStateRecovery				},	// 16
	{PwmStateRunning, 				Evt_UnderVoltageDetected,  	Action_RecoveryMode,   		PwmStateRecovery				},	// 17
	{PwmStateRunning, 				Evt_OverCurrentDetected,   	Action_RecoveryMode,   		PwmStateRecovery				},	// 18
	{PwmStateRunning, 				Evt_ShortCircuitDetected,  	Action_Shutdown,       		PwmStateHardStop				},	// 19
	{PwmStateRunning, 				Evt_ArcDetected,           	Action_Shutdown,       		PwmStateHardStop				},	// 20
	// ====== RECOVERY ======
	{PwmStateRecovery, 				Evt_TuningCommand,        	Action_Tune,           		PwmStateRunning					},	// 21
	{PwmStateRecovery, 				Evt_StopCommand,           	Action_SoftStop,       		PwmStateSoftStop				},	// 22
	{PwmStateRecovery, 				Evt_HardwareFault,        	Action_Shutdown,       		PwmStateHardStop				},	// 23
	{PwmStateRecovery, 				Evt_GridFaultDetected,    	Action_Shutdown,       		PwmStateHardStop				},	// 24
	{PwmStateRecovery, 				Evt_FaultCleared,         	Action_SoftRestart,    		PwmStateSoftStart				},	// 25
	// ====== SOFT STOP =====
	{PwmStateSoftStop, 				Evt_SoftStopDone,         	Action_EnterStandby,   		PwmStateStandby					},	// 26
	// ====== HARD STOP ======
	{PwmStateHardStop, 				Evt_FaultCleared,         	Action_ResetSystem,    		PwmStateStandby					},	// 27
};

const uint32_t stateTimingTable_us[PwmStateEND] = {
	[PwmStateStandby]        	= 1,   	// 50ms
	[PwmStateInit]           	= 1,    	// 1ms
	[PwmStateSoftStart]      	= 1,			// 1ms
	[PwmStateResonanceSweep]	= 50000,		//
	[PwmStateRunning]        	= 50000,    // 
	[PwmStateRecovery]        = 50000,		//
	[PwmStateSoftStop]       	= 50000,		//
	[PwmStateHardStop]				= 50000			//
};


PWM_State_Machine PWM_FSM_GetCurrentState(struct AppContext* app){
	return app->pwm.currentState;
}
//--------------------main
bool PWM_FSM_HandleEvent(struct AppContext* app, PWM_Event_t event){;
	for (size_t i = 0; i < TRANSITION_NUM; i++){
		const StateTransition_t* t = &transitions[i];
		if (t->currentState == app->pwm.currentState && t->event == event){
			bool ok = true;
			if(t->action){
				ok = t->action(app);
				if(ok){
					//action success(currentState, t->nextState, event);  // برای debug
					app->pwm.currentState = t->nextState;
				}
				else{
					errorCode = ERROR_CODE_Evt_Start + event;
				}
			}
			return ok; // چون یک ترنزیشن پیدا شده، ادامه نمی‌دیم
		}
	}
	return false;
}
//-----------------------------------------------------------
void PWM_FSM_Init(struct AppContext* app ){
	ADC_Context_init(&app->adc);
	// 1) مقداردهی اولیه‌ی state
	app->pwm.currentState = PwmStateStandby;
	// 2) پاک‌سازی فلگ‌های داخلی FSM
	// اگر از event queue استفاده می‌کنی:
	EventQueue_Clear(&app->queue);             // queue = empty
	// 3) آماده‌سازی زیرساخت تریگر زمان FSM
	//    این‌جا TIM2 را ری‌ست ولی نباید PWM/TIM1 را دست بزنی
	__HAL_TIM_SET_COUNTER(app->counterTimer, 0);
	HAL_TIM_Base_Start_IT(app->counterTimer);  // هر دوره یک update IRQ تولید می‌کند

	// 4) ست کردن یک‌بار stateMachineFlag برای kickstart
	EventQueue_Clear(&app->queue);
	app->fsm_tick_us = 0;
}
//---------------------------------------------------------------
bool EnqueueEvent(struct AppContext* app, PWM_Event_t evt){
	return EnqueueEvent_ctx(&app->queue, evt);
}
bool DequeueEvent(struct AppContext* app, PWM_Event_t* evt) {
	return DequeueEvent_ctx(&app->queue, evt);
}
//---------------------------------------
bool Action_None(struct AppContext* app){
	return true;// nothing to do
}
bool Action_EnterInit(struct AppContext* app){
  reset_PWM_control_variables(&app->pwm); 	//ok
  if(!clear_fault_flags()){				//ok
		//error
		return false;
	}
	if(!manual_PWM_Disable()){			//ok
		//error
		return false;
	}		
	if(!manual_Timers_Reset()){			//ok
		//error
		return false;
	}
	if(!manual_ADC_Enable(&app->adc)){
		return false;
	}//ok
	if(!Enable_ProtectionInterrupts()){	//ok
		//error
		return false;
	}
	__HAL_TIM_SET_COUNTER(app->counterTimer, 0);
	HAL_TIM_Base_Start_IT(app->counterTimer); // جاش ا
	return true;
}
bool Action_EnterSoftStart(struct AppContext* app){
	if(!manual_Timers_Enable()){
		//error code
		return false;
	}

	if(!set_PWM_frequency(PWM_SOFT_START_START_FREQ)){// 10kHz
		//error code
		return false;
	}
	HAL_PWM_SetDeadTime(PWM_START_DEAD_TIME); //255-> 14us 
	if(!manual_PWM_Enable()){
		//error code
		return false;
	}
	if(!Enable_ProtectionInterrupts()){
		//error code
		return false;
	}
	set_PWM_control_variables(&app->pwm);
	//Mechnical_Part_Handler(&mechParts[MECHANICAL_PART_Fan1],POWER_MODE_ON);
	//Mechnical_Part_Handler(&mechParts[MECHANICAL_PART_Fan2],POWER_MODE_ON);
	return true;
}
bool Action_StartSweep(struct AppContext* app){
	IC_Init(&app->ic);
	__HAL_TIM_CLEAR_FLAG(app->pwm.pwmTimer, TIM_FLAG_CC3 | TIM_FLAG_CC4 | TIM_FLAG_CC3OF | TIM_FLAG_CC4OF);
	if(HAL_TIM_IC_Start_IT(app->pwm.pwmTimer, TIM_CHANNEL_3) != HAL_OK){ //start input capture
		// error
		return false;
	}
	if(HAL_TIM_IC_Start_IT(app->pwm.pwmTimer, TIM_CHANNEL_4) != HAL_OK){ //start input capture
		// error
		return false;
	}
	return true;
}
bool Action_EnterRunning(struct AppContext* app){
	return true;
}
bool Action_RecoveryMode(struct AppContext* app){
	return true;
}
bool Action_Tune(struct AppContext* app){
	return true;
}
bool Action_SoftStop(struct AppContext* app){
	return true;
}
bool Action_FailSoftStop(struct AppContext* app){
	return true;
}
bool Action_Shutdown(struct AppContext* app){
	return true;
}
bool Action_EnterStandby(struct AppContext* app){
	
	return true;
}
bool Action_SoftRestart(struct AppContext* app){
	return true;
}
bool Action_ResetSystem(struct AppContext* app){
	return true;
}
//-------------------------------------------state functions
void stateStandby(struct AppContext* app){
	
}
void stateInit(struct AppContext* app){
	initMachine[initMode](app);
	EnqueueEvent(app, Evt_InitComplete);
	return;
}
void stateSoftStart(struct AppContext* app){
	softStartMachine[softStartMode](app);
	EnqueueEvent(app,Evt_SoftStartDone);
}
void stateResonanceSweep(struct AppContext* app){
	resonanceSweepMachine[resonanceSweepMode](app);
	EnqueueEvent(app,Evt_ResonanceFound);
	/*if(!captureReadyCh3 || !captureReadyCh4){
		return;
	}
	if(captureReadyCh3 && captureReadyCh4){
		HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_3);
		HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_4);
		//بررسي پايداري نتيجه ها   که در صورت پايدار نبودن افزايش توان و نمونه گيري مجدد
		// ميانگين گيري از نمونه ها 
		// پردازش نتايج 
		//   حرکت به سمت رزونانس (افزايش يا کاهش فرکانس 
		// تغيير فرکانس 
		// ريست فلگ ها و نمونه گيري مجدد
		HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3); //start input capture
		HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_4); //start input capture
		// چک کردن و قفل فرکانس  در غير اينصورت بازگشت و نمونه گيري مجدد
	}
	EnqueueEvent(Evt_ResonanceFound);*/
}
void stateRunning(struct AppContext* app){
	
}
void stateRecovery(struct AppContext* app){
	
}
void stateSoftStop(struct AppContext* app){
	
}
void stateHardStop(struct AppContext* app){
	
}
//-------------------------------------------------
const keyAction keyAct[PwmStateEND] = {
	[PwmStateStandby]        	=	stateStandby_keyCallback ,
	[PwmStateInit]           	=	stateInit_keyCallback ,																											
	[PwmStateSoftStart]      	=	stateSoftStart_keyCallback ,																											
	[PwmStateResonanceSweep]	= stateResonanceSweep_keyCallback ,
	[PwmStateRunning]        	= stateRunning_keyCallback ,
	[PwmStateRecovery]        = stateRecovery_keyCallback ,
	[PwmStateSoftStop]       	= stateSoftStop_keyCallback ,
	[PwmStateHardStop]				= stateHardStop_keyCallback
};
void stateStandby_keyCallback(struct AppContext* app){
	Key_Read(&app->keys[PWM_COMMMAND_KEY],1);
	if(app->keys[PWM_COMMMAND_KEY].state == KeyStatePressed){
		app->keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(app, Evt_StartCommand);
	}
}
void stateInit_keyCallback(struct AppContext* app){
	Key_Read(&app->keys[PWM_COMMMAND_KEY], 1);
	if (app->keys[PWM_COMMMAND_KEY].state == KeyStatePressed) {
		app->keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(app, Evt_StopCommand);
	}
}
void stateSoftStart_keyCallback(struct AppContext* app){
	Key_Read(&app->keys[PWM_COMMMAND_KEY], 1);
	if (app->keys[PWM_COMMMAND_KEY].state == KeyStatePressed) {
		app->keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(app, Evt_StartCommand);
	}
}
void stateResonanceSweep_keyCallback(struct AppContext* app){
	Key_Read(&app->keys[PWM_COMMMAND_KEY], 1);
	if (app->keys[PWM_COMMMAND_KEY].state == KeyStatePressed) {
		app->keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(app, Evt_StartCommand);
	}
}
void stateRunning_keyCallback(struct AppContext* app){
	Key_Read(&app->keys[PWM_COMMMAND_KEY], 1);
	if (app->keys[PWM_COMMMAND_KEY].state == KeyStatePressed) {
		app->keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(app, Evt_StartCommand);
	}
}
void stateRecovery_keyCallback(struct AppContext* app){
	Key_Read(&app->keys[PWM_COMMMAND_KEY], 1);
	if (app->keys[PWM_COMMMAND_KEY].state == KeyStatePressed) {
		app->keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(app, Evt_StartCommand);
	}
}
void stateSoftStop_keyCallback(struct AppContext* app){
	//do nothing
}
void stateHardStop_keyCallback(struct AppContext* app){
	//do nothing
}

//soft start state machine function

//Resonance Sweep state machine function
/*
void resonanceSweep_sampling			(void){
	//get input cpture sample for voltage and current signal of induction furnace
	
	//get current sample of inject channel for checking safety and calculating power
	if(currentSampleReady == false){
		INJECT_GET_SAMPLE();
	}
	//get voltage and temperture from dma_adc to checking safety and calculating power
	if(dmaSampleReady == false){
		DMA_GET_SAMPLE();
	}
	if(captureHndler[IC_CH3].ready && captureHndler[IC_CH4].ready && currentSampleReady == true && dmaSampleReady == true){
		resonanceSweepMode = RS_processing;
	}
}
void resonanceSweep_processing		(void){
	//get average of top value
}
void resonanceSweep_safatyCheck		(void){
	//safety check of top value
	//check power limit for this state
}

void resonanceSweep_settingChanges(void){
	//calculate phase diff
	//set new deadtime for tun power if needed
	//set new freq and back to sampling or Confirmation reso freq and to finishing
}
void resonanceSweep_Finishing			(void){
	EnqueueEvent(Evt_ResonanceFound);
}*/
//Running state machine function

//Recovery state machine function

//Soft Stop state machine function

//HardStop state machine function

//end of pwm_fsm.c