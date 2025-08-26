#ifndef __FSM_TICK_H__
#define __FSM_TICK_H__

#pragma once
#include "tim.h"
struct AppContext;

void FSM_Tick_Attach(struct AppContext* app);   // اتصال کانتکست
void FSM_Tick_OnPeriodElapsed(TIM_HandleTypeDef* htim); // فراخوانی از HAL cb

#endif //__FSM_TICK_H__