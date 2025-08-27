#ifndef __FSM_INIT_H__
#define __FSM_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

struct AppContext; // forward declaration

typedef enum{
	Init_connectPower				= 0,
	Init_DMA_Sampling,
	Init_DMA_Processing,
	Init_safatyCheck,
	Init_calibratingCurrent,
	Init_Finishing,
	Init_END,
}InitSub;

typedef void (*SM_Init_func)(struct AppContext* app);

void init_connectPower			(struct AppContext* app);
void init_DMA_Sampling			(struct AppContext* app);
void init_DMA_Processing		(struct AppContext* app);
void init_safatyCheck				(struct AppContext* app);
void init_calibratingCurrent(struct AppContext* app);
void init_Finishing					(struct AppContext* app);

extern const 		SM_Init_func initMachine[Init_END];
extern volatile InitSub initMode;

#ifdef __cplusplus
}
#endif
#endif//__FSM_INIT_H__