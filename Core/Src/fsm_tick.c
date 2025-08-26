#include "app_ctx.h"

extern const uint32_t stateTimingTable_us[];
static struct AppContext* s_app = {0};   // فقط در این فایل دیده می‌شود (نه global واقعی پروژه)

void FSM_Tick_Attach(struct AppContext* app){
    s_app = app;
}

void FSM_Tick_OnPeriodElapsed(TIM_HandleTypeDef* htim){
	if(s_app->fsm_tick_us++ >= stateTimingTable_us[s_app->pwm.currentState]){
		s_app->fsm_tick_us = 0;
		s_app->flags.tick = 1;
	}
	if(s_app->key_tick_us++ >= 50000){
		s_app->key_tick_us = 0;
		s_app->flags.keyRead = 1;
	}
	if(s_app->sevenSeg_tick_us++ >= 4000){
		s_app->sevenSeg_tick_us = 0;
		s_app->flags.segUpdate = 1;
	}
}
//end of fsm_tick.c