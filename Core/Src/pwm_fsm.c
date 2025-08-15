#include "pwm_fsm.h"
#include "pwm.h"
#include "tim.h"
#include "key.h"
#include "mechanical_part.h"
#include "adc.h"
volatile bool stateMachineFlag = false;
volatile Init_MODE initMode = Init_MODE_DMA_Sampling;
EventQueue_t eventQueue;	
KeyPinConfig keys[END_KEY];
ERROR_CODE errorCode = ERROR_CODE_None;
const adc_funk initMachine[Init_MODE_END] = {
	[Init_MODE_DMA_Sampling]				=	DMA_Sampling,
	[Init_MODE_DMA_Processing]			= DMA_Processing,
	[Init_MODE_safatyCheck]					= safatyCheck,
	[Init_MODE_calibratingCurrent]	= calibratingCurrent,
	[Init_MODE_adcDisable]					= adcDisable,
	[Init_MODE_initFinishing]				= initFinishing,
};
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
	{PwmStateResonanceSweep, 	Evt_ResonanceLost,  				Action_FailSoftStop,  		PwmStateSoftStop				},	// 8
	{PwmStateResonanceSweep, 	Evt_StopCommand,  					Action_FailSoftStop,  		PwmStateSoftStop				},	// 9
	{PwmStateResonanceSweep, 	Evt_HardwareFault,  				Action_Shutdown,      		PwmStateHardStop				},	// 10
	// ====== RUNNING ======
	{PwmStateRunning, 				Evt_TargetPowerReached,    	Action_None,           		PwmStateRunning					},	// 11
	{PwmStateRunning, 				Evt_StopCommand,           	Action_SoftStop,       		PwmStateSoftStop				},	// 12
	{PwmStateRunning, 				Evt_ResonanceLost,         	Action_RecoveryMode,   		PwmStateRecovery				},	// 13
	{PwmStateRunning, 				Evt_OverPowerDetected,     	Action_RecoveryMode,   		PwmStateRecovery				},	// 14
	{PwmStateRunning, 				Evt_OverTempDetected,      	Action_RecoveryMode,   		PwmStateRecovery				},	// 15
	{PwmStateRunning, 				Evt_UnderVoltageDetected,  	Action_RecoveryMode,   		PwmStateRecovery				},	// 16
	{PwmStateRunning, 				Evt_OverCurrentDetected,   	Action_RecoveryMode,   		PwmStateRecovery				},	// 17
	{PwmStateRunning, 				Evt_ShortCircuitDetected,  	Action_Shutdown,       		PwmStateHardStop				},	// 18
	{PwmStateRunning, 				Evt_ArcDetected,           	Action_Shutdown,       		PwmStateHardStop				},	// 19
	// ====== RECOVERY ======
	{PwmStateRecovery, 				Evt_TuningCommand,        	Action_Tune,           		PwmStateRunning					},	// 20
	{PwmStateRecovery, 				Evt_StopCommand,           	Action_SoftStop,       		PwmStateSoftStop				},	// 21
	{PwmStateRecovery, 				Evt_HardwareFault,        	Action_Shutdown,       		PwmStateHardStop				},	// 22
	{PwmStateRecovery, 				Evt_GridFaultDetected,    	Action_Shutdown,       		PwmStateHardStop				},	// 23
	{PwmStateRecovery, 				Evt_FaultCleared,         	Action_SoftRestart,    		PwmStateSoftStart				},	// 24
	// ====== SOFT STOP ======
	{PwmStateSoftStop, 				Evt_SoftStopDone,         	Action_EnterStandby,   		PwmStateStandby					},	// 25
	// ====== HARD STOP ======
	{PwmStateHardStop, 				Evt_FaultCleared,         	Action_ResetSystem,    		PwmStateStandby					},	// 26
};

const uint32_t stateTimingTable_us[PwmStateEND] = {
	[PwmStateStandby]        	= 50000,   	// 50ms
	[PwmStateInit]           	= 1000,    	// 1ms
	[PwmStateSoftStart]      	= 1000,			// 1ms
	[PwmStateResonanceSweep]	= 50000,		//
	[PwmStateRunning]        	= 50000,    // 
	[PwmStateRecovery]        = 50000,		//
	[PwmStateSoftStop]       	= 50000,		//
	[PwmStateHardStop]				= 50000			//
};


PWM_State_Machine PWM_FSM_GetCurrentState(void){
	return pwmState.currentState;
}
void EventQueue_Clear(void){
	eventQueue.head = 0;
	eventQueue.tail = 0;
}
void reset_fsm_control_flags(void){
	stateMachineFlag = false;
}
//--------------------main
bool PWM_FSM_HandleEvent(PWM_Event_t event){;
	for (size_t i = 0; i < TRANSITION_NUM; i++){
		const StateTransition_t* t = &transitions[i];
		if (t->currentState == pwmState.currentState && t->event == event){
			bool ok = true;
			if(t->action){
				ok = t->action();
				if(ok){
					//action success(currentState, t->nextState, event);  // برای debug
					pwmState.currentState = t->nextState;
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
void PWM_FSM_Init(void){
	// 1) مقداردهی اولیه‌ی state
	pwmState.currentState = PwmStateStandby;
	// 2) پاک‌سازی فلگ‌های داخلی FSM
	reset_fsm_control_flags();          // فقط فلگ‌ها
	// اگر از event queue استفاده می‌کنی:
	EventQueue_Clear();             // queue = empty
	// 3) آماده‌سازی زیرساخت تریگر زمان FSM
	//    این‌جا TIM2 را ری‌ست ولی نباید PWM/TIM1 را دست بزنی
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	HAL_TIM_Base_Start_IT(&htim2);  // هر دوره یک update IRQ تولید می‌کند

	// 4) ست کردن یک‌بار stateMachineFlag برای kickstart
	stateMachineFlag = true;
	fsm_tick_us = 0;
}
//---------------------------------------------------------------
bool EnqueueEvent(PWM_Event_t evt){
	uint8_t next = (eventQueue.head + 1) % EVENT_QUEUE_SIZE;
	if (next == eventQueue.tail){
		return false; // پر شده
	}
	eventQueue.queue[eventQueue.head] = evt;
	eventQueue.head = next;
	return true;
}

bool DequeueEvent(PWM_Event_t* evt) {
	if (eventQueue.head == eventQueue.tail){
		return false; // خالی
	}
	*evt = eventQueue.queue[eventQueue.tail];
	eventQueue.tail = (eventQueue.tail + 1) % EVENT_QUEUE_SIZE;
	return true;
}
//---------------------------------------
bool Action_None(void){
	return true;// nothing to do
}
bool Action_EnterInit(void){
  reset_PWM_control_variables(); 	//ok
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
	if(!Enable_ProtectionInterrupts()){	//ok
		//error
		return false;
	}
	if(!manual_ADC_Enable()){
		return false;
	}//ok
	adc_dma_done = false;
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	HAL_TIM_Base_Start_IT(&htim2);
	return true;
}
bool Action_EnterSoftStart(void){
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
	
	if(!manual_ADC_Enable()){
		//error code
		return false;
	}
	
	if(!Enable_ProtectionInterrupts()){
		//error code
		return false;
	}
	set_PWM_control_variables(&pwmState);
	Mechnical_Part_Handler(&mechParts[MECHANICAL_PART_Fan1],POWER_MODE_ON);
	Mechnical_Part_Handler(&mechParts[MECHANICAL_PART_Fan2],POWER_MODE_ON);
	return true;
}
bool Action_StartSweep(void){
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_3); //start input capture
	HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_4); //start input capture
	
	return true;
}  
bool Action_EnterRunning(void){
	return true;
}
bool Action_RecoveryMode(void){
	return true;
}
bool Action_Tune(void){
	return true;
}
bool Action_SoftStop(void){
	return true;
}
bool Action_FailSoftStop(void){
	return true;
}
bool Action_Shutdown(void){
	return true;
}
bool Action_EnterStandby(void){
	
	return true;
}
bool Action_SoftRestart(void){
	return true;
}
bool Action_ResetSystem(void){
	return true;
}
//-------------------------------------------state functions
void stateStandby(void){
	
}
void stateInit(void){
	initMachine[initMode]();
	EnqueueEvent(Evt_InitComplete);
	return;
}
void stateSoftStart(void){
	if(!pwmState.flags.freqRampDone){
		if(!softStart_set_freq_ramp()){
			return;
		}
		pwmState.flags.freqRampDone = PWM_TRUE;
	}
	else if(!softStart_tun_power()){
		return;
	}
	EnqueueEvent(Evt_SoftStartDone);
}
void stateResonanceSweep(void){
	if(!captureReadyCh3 || !captureReadyCh4){
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
	EnqueueEvent(Evt_ResonanceFound);
}
void stateRunning(void){
	
}
void stateRecovery(void){
	
}
void stateSoftStop(void){
	
}
void stateHardStop(void){
	
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
void stateStandby_keyCallback(void){
	Key_Read(&keys[PWM_COMMMAND_KEY],1);
	if(keys[PWM_COMMMAND_KEY].state == KeyStatePressed){
		keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(Evt_StartCommand);
	}
}
void stateInit_keyCallback(void){
	Key_Read(&keys[PWM_COMMMAND_KEY], 1);
	if (keys[PWM_COMMMAND_KEY].state == KeyStatePressed) {
		keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(Evt_StopCommand);
	}
}
void stateSoftStart_keyCallback(void){
	Key_Read(&keys[PWM_COMMMAND_KEY], 1);
	if (keys[PWM_COMMMAND_KEY].state == KeyStatePressed) {
		keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(Evt_StopCommand);
	}
}
void stateResonanceSweep_keyCallback(void){
	Key_Read(&keys[PWM_COMMMAND_KEY], 1);
	if (keys[PWM_COMMMAND_KEY].state == KeyStatePressed) {
		keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(Evt_StopCommand);
	}
}
void stateRunning_keyCallback(void){
	Key_Read(&keys[PWM_COMMMAND_KEY], 1);
	if (keys[PWM_COMMMAND_KEY].state == KeyStatePressed) {
		keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(Evt_StopCommand);
	}
}
void stateRecovery_keyCallback(void){
	Key_Read(&keys[PWM_COMMMAND_KEY], 1);
	if (keys[PWM_COMMMAND_KEY].state == KeyStatePressed) {
		keys[PWM_COMMMAND_KEY].state = KeyStateOnNone;
		EnqueueEvent(Evt_StopCommand);
	}
}
void stateSoftStop_keyCallback(void){
	//do nothing
}
void stateHardStop_keyCallback(void){
	//do nothing
}
//init state machine function
void DMA_Sampling				(void){
	if(adc_dma_done && dmaSampleCounter < SAMPLE_NUM){
		adc_dma_done = false;
		voltageSample[dmaSampleCounter] = adc_dma_buffer[ADC_IDX_VBUS];
		temp1Sample[dmaSampleCounter] = adc_dma_buffer[ADC_IDX_TEMP_CH1];
		temp2Sample[dmaSampleCounter] = adc_dma_buffer[ADC_IDX_TEMP_CH2];
		dmaSampleCounter++;
		return;
	}
	else if(dmaSampleCounter >= SAMPLE_NUM){
		if(HAL_ADC_Stop_DMA(ADC_UNIT) != HAL_OK){
			return;
		}
		dmaSampleCounter = 0;
		initMode = Init_MODE_DMA_Processing;
		return;
	}
	return;
}
void DMA_Processing			(void){
	uint32_t sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += voltageSample[i];
	}
	dmaSampleMean[ADC_IDX_VBUS] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
	sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += temp1Sample[i];
	}
	dmaSampleMean[ADC_IDX_TEMP_CH1] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
	sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += temp2Sample[i];
	}
	dmaSampleMean[ADC_IDX_TEMP_CH2] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
	initMode = Init_MODE_safatyCheck;
	return;
}
void safatyCheck				(void){
	if(!DC_Voltage_Safety_Checker()){
		//error
		return;
	}
	if(!Temperture_Safety_Checker()){
		//error
		return;
	}
	initMode = Init_MODE_calibratingCurrent;
	return;
}
void calibratingCurrent	(void){
	if(!ADC_currentChannelCalibrate()){
		//error
		return;
	}
	initMode = Init_MODE_adcDisable;
	return;
}
void adcDisable					(void){
	manual_ADC_Disable();
	initMode = Init_MODE_initFinishing;
	return;
}
void initFinishing			(void){
	//do noting
}
//soft start state machine function

//Resonance Sweep state machine function

//Running state machine function

//Recovery state machine function

//Soft Stop state machine function

//HardStop state machine function

//end of pwm_fsm.c