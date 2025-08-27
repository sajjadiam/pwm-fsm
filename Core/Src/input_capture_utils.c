#include "app_ctx.h"
#include "tim.h"
#include <string.h>

static IC_Context* s_ic;
void IC_AttachContext(IC_Context* ctx){
	s_ic = ctx; 
}
void captureUnitInit(IC_Handler *cu ,TIM_HandleTypeDef *htim ,uint32_t channel){
	cu->htim = htim;
	cu->ch = channel;
	cu->armed = 1;
	cu->ready = 0;
	cu->count = 0;
	cu->avg = 0;
	memset(cu->buff,0,sizeof cu->buff);
}
void IC_Init(IC_Context* ctx){
	captureUnitInit(&ctx->ch[IC_CH_I],&htim1,TIM_CHANNEL_3);
	captureUnitInit(&ctx->ch[IC_CH_V],&htim1,TIM_CHANNEL_4);
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM1){
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3){
			IC_Handler* cu = &s_ic->ch[IC_CH_I];
			if (cu->armed && cu->count < SAMPLE_NUM) {
				cu->buff[cu->count++] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
				if (cu->count >= SAMPLE_NUM){ 
					cu->armed = 0;
					cu->ready = 1; 
					cu->count = 0;
				}
			}
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4){
			IC_Handler* cu = &s_ic->ch[IC_CH_V];
			if (cu->armed && cu->count < SAMPLE_NUM) {
				cu->buff[cu->count++] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
				if (cu->count >= SAMPLE_NUM){ 
					cu->armed = 0;
					cu->ready = 1; 
					cu->count = 0;
				}
			}
		}
	}
}
void IC_processSample(IC_Context* ctx,uint16_t len){
	IC_Handler* cu = ctx->ch;
	uint8_t i;
	const uint8_t sampleNumber = SAMPLE_NUM - 1;
	while(len-- > 0){
		uint32_t sampleSum = 0;
		for(i = 0;i < sampleNumber;i++){
			sampleSum += ((cu->buff[i + 1] > cu->buff[i]) ? (cu->buff[i + 1] - cu->buff[i])
				: ((cu->htim->Init.Period + 1u) - cu->buff[i] + cu->buff[i + 1]));
		}
		cu->avg = sampleSum / sampleNumber;
		cu->ready = 0;
		cu++;
	}
}

//end of input_capture_utils.c