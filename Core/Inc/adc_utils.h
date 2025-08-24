#ifndef __ADC_UTILS_H__
#define __ADC_UTILS_H__


#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include <stdbool.h>
#include "adc_utils_config.h"

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
	uint16_t dma_buffer[DMA_Index_End];
	//samples
	uint16_t currentSample[SAMPLE_NUM_MAX];
	uint16_t voltageSample[SAMPLE_NUM_MAX];
	uint16_t temp1Sample[SAMPLE_NUM_MAX];
	uint16_t temp2Sample[SAMPLE_NUM_MAX];
	//mean of samples
	uint16_t sampleMean[ADC_Channel_End];
	//Offsets
	uint16_t currentOffset;
	//counters
	Counter currentSampleCounter;
	Counter dmaSampleCounter;
	//flags
	struct{
		volatile uint8_t dmaSampleReady			:1;
		volatile uint8_t currentSampleReady	:1;
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
	CC_Finishing,
	CC_SetAWD,
	CC_End,
}CurrentCalibrateSub;
typedef void (*adc_funk)(ADC_Context* ctx);

//extern variables
extern ADC_Context adcCtx;
//functions
void ADC_Context_init		(ADC_Context* ctx);
bool manual_ADC_Enable	(ADC_Context* ctx);
bool manual_ADC_Disable	(ADC_Context* ctx);

//----------------------------------



bool DC_Voltage_Safety_Checker(void);
bool Temperture_Safety_Checker(void);
float ADC_to_voltage(uint16_t adc);
float ADC_to_temperture(uint16_t adc);
float ADC_to_current(uint16_t adc);

uint32_t ADC_inject_trigger(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef ADC_inject_set_trigger(ADC_HandleTypeDef* hadc, uint32_t injectTrigger);
bool ADC_currentChannelCalibrate(void);
void ADC_GetTrig					(void);
void ADC_SetTrig					(void);
void ADC_Start						(void);
void ADC_Sampling					(void);
void ADC_Processing				(void);
void ADC_Measuringccuracy	(void);
void ADC_ResetTrig				(void);
void ADC_SetAWD						(void);
void ADC_Finishing				(void);
//----------------------------
void DMA_Sampling				(void);
void DMA_Processing			(void);
void safatyCheck				(void);
void calibratingCurrent	(void);
void adcDisable					(void);
void initFinishing			(void);
//----------------------------------
void DMA_GET_SAMPLE(void);
void INJECT_GET_SAMPLE(void);
#endif // ADC_UTILS_H
