#ifndef __ADC_UTILS_H__
#define __ADC_UTILS_H__


#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include <stdbool.h>
#include "adc_utils_config.h"
//includes

//inline func
static inline uint16_t CurrentA_to_ADCcounts(float I_amp) {
	if(V_REF == 0){
		return 0;
	}
	return (I_amp * R_SHUNT * G_TOTAL) * (ADC_MAX / V_REF);
}

static inline float ADCcounts_to_CurrentA(uint16_t counts) {
	return (float)(counts * V_REF) / (ADC_MAX * R_SHUNT * G_TOTAL);
}

static inline float ADCcounts_to_VoltageV(uint16_t counts) {
	return (counts * V_REF * VOLTAGE_GAIN) / ADC_MAX;
}

//typedefs
typedef uint16_t Counter;
typedef struct{
	ADC_HandleTypeDef* 	adc;
	uint16_t 						dma_buffer[DMA_Index_End];
	//samples
	uint16_t 						currentSample[SAMPLE_NUM_MAX];
	uint16_t 						voltageSample[SAMPLE_NUM_MAX];
	uint16_t 						temp1Sample[SAMPLE_NUM_MAX];
	uint16_t 						temp2Sample[SAMPLE_NUM_MAX];
	//mean of samples
	uint16_t 						sampleMean[ADC_Channel_End];
	//Offsets
	uint16_t 						currentOffset;
	//counters
	Counter 						currentSampleCounter;
	Counter 						dmaSampleCounter;
	uint8_t 						oc_hits;
	//flags
	struct{
		volatile uint8_t 	dmaSampleReady			:1;
		volatile uint8_t 	currentSampleReady	:1;
	};
}ADC_Context;
typedef enum {
	CC_GetTrig	= 0,
	CC_SetTrig,
	CC_Start,
	CC_Sampling,
	CC_Processing,
	CC_Measuringccuracy,
	CC_ResetTrig,
	CC_SetAWD,
	CC_Finishing,
	CC_End,
}CurrentCalibrateSub;
typedef void (*adc_funk)(ADC_Context* ctx);

//extern variables
//extern ADC_Context adcCtx;
//functions
void ADC_Context_init		(ADC_Context* ctx);
bool manual_ADC_Enable	(ADC_Context* ctx);
bool manual_ADC_Disable	(ADC_Context* ctx);

//----------------------------------



bool DC_Voltage_Safety_Checker(ADC_Context* ctx);
bool Temperture_Safety_Checker(ADC_Context* ctx);
//-----------------------------------------
float ADC_to_voltage(uint16_t adc);
float ADC_to_temperture(uint16_t adc);
float ADC_to_current(uint16_t adc);
//----------------------------------------------------
uint32_t ADC_inject_trigger(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef ADC_inject_set_trigger(ADC_HandleTypeDef* hadc, uint32_t injectTrigger);
//------------------------------------------------
void CC_Func_GetTrig					(ADC_Context* ctx);
void CC_Func_SetTrig					(ADC_Context* ctx);
void CC_Func_Start						(ADC_Context* ctx);
void CC_Func_Sampling					(ADC_Context* ctx);
void CC_Func_Processing				(ADC_Context* ctx);
void CC_Func_Measuringccuracy	(ADC_Context* ctx);
void CC_Func_ResetTrig				(ADC_Context* ctx);
void CC_Func_SetAWD						(ADC_Context* ctx);
void CC_Func_Finishing				(ADC_Context* ctx);
//----------------------------
bool DMA_Sampling				(ADC_Context* ctx);
bool INJECT_Sampling		(ADC_Context* ctx);
bool DMA_Processing			(ADC_Context* ctx);
bool INJECT_Processing	(ADC_Context* ctx);
bool ADC_Processing			(ADC_Context* ctx);
bool safatyCheck				(ADC_Context* ctx);
bool calibratingCurrent	(ADC_Context* ctx);
bool adcDisable					(ADC_Context* ctx);
bool initFinishing			(ADC_Context* ctx);
//----------------------------------
void ADC_AttachContext(ADC_Context* ctx);
#endif // ADC_UTILS_H
