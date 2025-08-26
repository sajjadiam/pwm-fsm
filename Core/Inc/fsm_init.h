#ifndef __FSM_INIT_H__
#define __FSM_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	Init_DMA_Sampling			= 0,
	Init_DMA_Processing,
	Init_safatyCheck,
	Init_calibratingCurrent,
	Init_adcDisable,
//	Init_Finishing,
	Init_END,
}InitSub;

typedef void (*SM_Init_func)(void);

void init_DMA_Sampling			(void);
void init_DMA_Processing		(void);
void init_safatyCheck				(void);
void init_calibratingCurrent(void);
void init_adcDisable				(void);
//void init_Finishing					(void);

extern const 		SM_Init_func initMachine[Init_END];
extern volatile InitSub initMode;

#ifdef __cplusplus
}
#endif
#endif//__FSM_INIT_H__