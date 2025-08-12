#ifndef ADC_UTILS_H
#define ADC_UTILS_H


#include "stm32f1xx_hal.h"
#include <stdbool.h>

#define ADC_IDX_VBUS        	0
#define ADC_IDX_TEMP_CH1    	1
#define ADC_IDX_TEMP_CH2    	2
#define ADC_DMA_CHANNEL_COUNT 3



#define VOLTAGE_GAIN					122 // Practical measured value

#define R0 				10000.0f      	// 10KO at 25°C
#define B 				3950.0f        	// B value
#define T0 				298.15f      		// 25°C in Kelvin
#define R_FIXED 	10000.0f 				// ?????? ???? ???? ????? ???????
#define ADC_MAX 	4095.0f  				// ??? ADC 12 ???? ????
#define V_REF 		3.3f       			// ????? ???? ADC

#define OPAMP_GAIN						19.5f
#define ISO_GAIN							8
#define TOT_GAIN							(OPAMP_GAIN * ISO_GAIN)

#define SHUNT_RESISTOR_VALUE	0.001
#define MAX_CURRENT						12.0f

#define V_PER_A        		0.040f          // 40 mV/A (GAIN TLV=10, AMC single=×4)
#define COUNTS_PER_V   		(ADC_MAX/V_REF)
#define COUNTS_PER_A   		(V_PER_A * COUNTS_PER_V)
#define SAMPLE_NUM				20
#define ADC_CURRENT_UNIT	&hadc1
//uint16_t zero_code = measure_adc_zero(); // ??????? ??? ????? ?? ???????
//float    I_limit_A  = 12.0f;

//uint16_t awd_high = zero_code + (uint16_t)(COUNTS_PER_A * I_limit_A + 0.5f);
//----------------------------------
typedef void (*adc_funk)(void);
typedef enum {
	ADC_Current_Calibrate_Mode_GetTrig	= 0,
	ADC_Current_Calibrate_Mode_SetTrig,
	ADC_Current_Calibrate_Mode_Start,
	ADC_Current_Calibrate_Mode_Sampling,
	ADC_Current_Calibrate_Mode_Processing,
	ADC_Current_Calibrate_Mode_ResetTrig,
	ADC_Current_Calibrate_Mode_Finishing,
	ADC_Current_Calibrate_Mode_End,
}ADC_Current_Calibrate_Mode;
//----------------------------------
extern uint16_t adc_dma_buffer[ADC_DMA_CHANNEL_COUNT];
extern volatile bool adc_dma_done;
extern volatile bool adc_inject_done;
extern uint32_t InjectTrigger;
extern uint16_t currentSample[SAMPLE_NUM];
extern uint16_t currentSampleCounter;
extern uint32_t currentOffset;
extern adc_funk calibrateCurrentOffset_machine[ADC_Current_Calibrate_Mode_End];
extern ADC_Current_Calibrate_Mode calibrateMode;

bool manual_ADC_Enable(void);
bool manual_ADC_Disable(void);
bool DC_Voltage_Safety_Checker(void);
bool Temperture_Safety_Checker(void);
float ADC_to_voltage(uint16_t adc);
float ADC_to_temperture(uint16_t adc);
float ADC_to_current(uint16_t adc);

uint32_t ADC_inject_trigger(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef ADC_inject_set_trigger(ADC_HandleTypeDef* hadc, uint32_t injectTrigger);
uint16_t measure_adc_zero(void);
HAL_StatusTypeDef ADC_currentChannelCalibrate(void);
void ADC_GetTrig		(void);
void ADC_SetTrig		(void);
void ADC_Start			(void);
void ADC_Sampling		(void);
void ADC_Processing	(void);
void ADC_ResetTrig	(void);
void ADC_Finishing	(void);



#endif // ADC_UTILS_H
