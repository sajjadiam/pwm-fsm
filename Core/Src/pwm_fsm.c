#include "pwm_fsm.h"
#include "pwm.h"
#include "tim.h"
#include "key.h"
#include "mechanical_part.h"

volatile bool stateMachineFlag = false;
EventQueue_t eventQueue;	
KeyPinConfig keys[END_KEY];
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
	if(!adc_dma_done){
		return; // هنوز داده ADC نرسیده
  }
	if(!DC_Voltage_Safety_Checker() || !Temperture_Safety_Checker()){
		// مقادیر unsafe هستن، خطا بده
		// PWM_FSM_HandleEvent(Evt_OverTempDetected) یا مشابه
		return;
	}
	manual_ADC_Disable();
	EnqueueEvent(Evt_InitComplete);
}
void stateSoftStart(void){
	pwmState.voltage = ADC_to_voltage(adc_dma_buffer[ADC_IDX_VBUS]);    // ولتاژ باس
//	pwmState.current = ADC_to_current(adc_dma_buffer[ADC_IDX_CURRENT]); // جریان
	pwmState.currentPower = pwmState.voltage * pwmState.current;        // توان لحظه‌ای

	/* 2) کنترل Dead‑Time بر اساس محدوده توان */
	// اگر توان از سقف بالاتر رفت، Dead‑Time را زیاد کن (پالس باریک‌تر و توان کمتر)
	if(pwmState.currentPower > PWM_SOFT_START_UPPER_LIMIT_POWER && pwmState.currentDeadTime < PWM_END_DEAD_TIME){
		pwmState.currentDeadTime++;
		HAL_PWM_SetDeadTime(pwmState.currentDeadTime);
	}
	// اگر توان از کف پایین‌تر رفت و هنوز به حداقل Dead‑Time نرسیده‌ایم، Dead‑Time را کم کن
	else if(pwmState.currentPower < PWM_SOFT_START_LOWER_LIMIT_POWER && pwmState.currentDeadTime > PWM_START_DEAD_TIME){
		pwmState.currentDeadTime--;
		HAL_PWM_SetDeadTime(pwmState.currentDeadTime);
	}
	// اگر توان در محدودهٔ مجاز بود، به رَمپ خطی ادامه بده
	else if(pwmState.currentDeadTime > pwmState.targetDeadTime){
		pwmState.currentDeadTime--;
		HAL_PWM_SetDeadTime(pwmState.currentDeadTime);
	}

	/* 3) محاسبه فرکانس متناسب با Dead‑Time جدید و تنظیم PWM */
	// نگاشت خطی بین 10kHz-20kHz و DT=255-223
	// ΔDT = PWM_START_DEAD_TIME - PWM_END_DEAD_TIME = 255-223 = 32
	// Δf  = PWM_END_SOFT_START_FREQ - PWM_START_SOFT_START_FREQ = 20000-10000 = 10000
	// f = f_start + ((DT_start - DT_now) * Δf / ΔDT)
	uint16_t newFreq = (uint16_t)(
			PWM_SOFT_START_START_FREQ +
			(PWM_START_DEAD_TIME - pwmState.currentDeadTime) *
			(PWM_SOFT_START_END_FREQ - PWM_SOFT_START_START_FREQ) /
			(PWM_START_DEAD_TIME - PWM_END_DEAD_TIME)
	);
	// فقط اگر فرکانس واقعاً تغییر کرده آن را ست کن
	if (newFreq != pwmState.currentFreq && newFreq <= pwmState.targetFreq) {
		pwmState.currentFreq = newFreq;
		set_PWM_frequency(pwmState.currentFreq);
	}

	/* 4) بررسی شرایط پایان Soft‑Start */
	// وقتی به فرکانس هدف و Dead‑Time هدف رسیدیم، Soft‑Start تمام شده است
	if(pwmState.currentDeadTime <= pwmState.targetDeadTime && pwmState.currentFreq >= pwmState.targetFreq){
		EnqueueEvent(Evt_SoftStartDone);
	}
}
void stateResonanceSweep(void){
	
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