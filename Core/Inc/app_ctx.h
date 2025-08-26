#ifndef __APP_CTX_H__
#define __APP_CTX_H__

#include "pwm.h"
#include "adc_utils.h"
#include "input_capture_utils.h"
#include "key.h"
#include "event_queue.h"
#include "fsm_tick.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    volatile unsigned tick      :1;
    volatile unsigned keyRead   :1;
    volatile unsigned segUpdate :1;
    volatile unsigned hardFault :1;
}AppFlags;

typedef struct AppContext{
	// وضعیت PWM (جایگزین pwmState سراسری)
	PWM_State_t pwm;

	// کانتکست‌های زیرسیستم‌ها
	ADC_Context adc;
	IC_Context  ic;

	// صف رویداد و کلیدها
	EventQueue_t queue;
	KeyPinConfig keys[END_KEY];

	// فلگ‌ها و تایمر FSM
	AppFlags flags;
	
	TIM_HandleTypeDef* counterTimer;
	uint32_t fsm_tick_us; // جایگزین fsm_tick_us سراسری
	uint32_t key_tick_us;
	uint32_t sevenSeg_tick_us;
}AppContext;
#ifdef __cplusplus
}
#endif
#endif //__APP_CTX_H__