#ifndef ADC_UTILS_H
#define ADC_UTILS_H


#include "stm32f1xx_hal.h"
#include <stdbool.h>

#define ADC_IDX_VBUS        	0
#define ADC_IDX_TEMP_CH1    	1
#define ADC_IDX_TEMP_CH2    	2
#define ADC_DMA_CHANNEL_COUNT 3

#define CURRENT_GAIN					18

#define VOLTAGE_GAIN					129 // andaze giri daghigh

#define R0 				10000.0      // 10KO at 25°C
#define B 				3950.0        // B value
#define T0 				298.15       // 25°C in Kelvin
#define R_FIXED 	10000.0 // ?????? ???? ???? ????? ???????
#define ADC_MAX 	4095.0  // ??? ADC 12 ???? ????
#define V_REF 		3.3       // ????? ???? ADC

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
