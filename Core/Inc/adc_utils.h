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

#define OPAMP_GAIN						10
#define OPAMP_OUTPUT_OFFSET		(0.0003 * 10)
#define ISO_GAIN							4

#define SHUNT_RESISTOR_VALUE	0.001
#define MAX_CURRENT						12.0f

#define V_PER_A        0.040f          // 40 mV/A (GAIN TLV=10, AMC single=×4)
#define COUNTS_PER_V   (ADC_MAX/V_REF)
#define COUNTS_PER_A   (V_PER_A * COUNTS_PER_V)

//uint16_t zero_code = measure_adc_zero(); // ??????? ??? ????? ?? ???????
//float    I_limit_A  = 12.0f;

//uint16_t awd_high = zero_code + (uint16_t)(COUNTS_PER_A * I_limit_A + 0.5f);

extern uint16_t adc_current_buffer;
extern uint16_t adc_dma_buffer[ADC_DMA_CHANNEL_COUNT];
extern volatile bool adc_dma_done;

bool manual_ADC_Enable(void);
bool manual_ADC_Disable(void);
bool DC_Voltage_Safety_Checker(void);
bool Temperture_Safety_Checker(void);
float ADC_to_voltage(uint16_t adc);
float ADC_to_temperture(uint16_t adc);
float ADC_to_current(uint16_t adc);
#endif // ADC_UTILS_H
