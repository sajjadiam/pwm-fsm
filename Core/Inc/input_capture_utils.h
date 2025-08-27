#ifndef __INPUT_CAPTURE_UTILS_H__
#define __INPUT_CAPTURE_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "stm32f1xx_hal.h"

#define SAMPLE_NUM_MAX	50
#define SAMPLE_NUM			20

typedef enum{
	IC_CH_I	= 0,
	IC_CH_V,
	IC_END,
}IC_CH;
typedef struct{
	TIM_HandleTypeDef 	*htim;
	uint32_t						ch;
	uint32_t 						buff[SAMPLE_NUM_MAX];
	volatile uint16_t 	count;
	volatile uint16_t		avg;
	struct{
		volatile 					uint8_t armed		:1;
		volatile 					uint8_t ready		:1;
	};
}IC_Handler;

typedef struct{
	IC_Handler ch[IC_END];
}IC_Context;

void IC_AttachContext(IC_Context* ctx);
void captureUnitInit(IC_Handler *cu ,TIM_HandleTypeDef *htim ,uint32_t channel);
void IC_Init(IC_Context* ctx);
void IC_getSample(IC_Context* ctx ,uint16_t len);
void IC_processSample(IC_Context* ctx ,uint16_t len);



#ifdef __cplusplus
}
#endif

#endif //__INPUT_CAPTURE_UTILS_H__