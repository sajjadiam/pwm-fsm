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
	return ((float)counts * V_REF) / (ADC_MAX * R_SHUNT * G_TOTAL);
}
static inline float ADCcounts_to_VoltageV(uint16_t counts) {
	return ((float)counts * V_REF * VOLTAGE_GAIN) / ADC_MAX;
}

//----------------------------------
typedef void (*adc_funk)(void);
typedef enum {
	ADC_Current_Calibrate_Mode_GetTrig	= 0,
	ADC_Current_Calibrate_Mode_SetTrig,
	ADC_Current_Calibrate_Mode_Start,
	ADC_Current_Calibrate_Mode_Sampling,
	ADC_Current_Calibrate_Mode_Processing,
	ADC_Current_Calibrate_Mode_Measuringccuracy,
	ADC_Current_Calibrate_Mode_ResetTrig,
	ADC_Current_Calibrate_Mode_Finishing,
	ADC_Current_Calibrate_Mode_SetAWD,
	ADC_Current_Calibrate_Mode_End,
}ADC_Current_Calibrate_Mode;
//----------------------------------
extern uint16_t adc_dma_buffer[ADC_DMA_CHANNEL_COUNT];
extern uint16_t voltageSample[SAMPLE_NUM_MAX];
extern uint16_t temp1Sample[SAMPLE_NUM_MAX];
extern uint16_t temp2Sample[SAMPLE_NUM_MAX];
extern uint16_t dmaSampleMean[ADC_DMA_CHANNEL_COUNT];
extern volatile bool adc_dma_done;
extern volatile bool adc_inject_done;
extern uint32_t InjectTrigger;
extern uint16_t currentSample[SAMPLE_NUM_MAX];
extern uint16_t currentSampleCounter;
extern uint16_t dmaSampleCounter;
extern uint16_t currentOffset;
extern adc_funk calibrateCurrentOffset_machine[ADC_Current_Calibrate_Mode_End];
extern ADC_Current_Calibrate_Mode calibrateMode;
extern uint16_t currentSmpleMean;
bool manual_ADC_Enable(void);
bool manual_ADC_Disable(void);
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
