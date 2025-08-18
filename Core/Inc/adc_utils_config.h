#ifndef __ADC_UTILS_CONFIG__
#define __ADC_UTILS_CONFIG__

//DMA CONFIG
#define ADC_IDX_VBUS        	0
#define ADC_IDX_TEMP_CH1    	1
#define ADC_IDX_TEMP_CH2    	2
#define ADC_DMA_CHANNEL_COUNT 3
//VOLTAGE GAIN
#define VOLTAGE_GAIN					122 // Practical measured value
//NTC conversion
#define R0 				10000.0f      	// 10KO at 25°C
#define B 				3950.0f        	// B value
#define T0 				298.15f      		// 25°C in Kelvin
#define R_FIXED 	10000.0f 				// series resistor
//ADC BASE CONFIG
#define ADC_MAX 	4095U  				// 12 bit ADC 
#define V_REF 		3.3f       			// ADC refrense voltage
#define SAMPLE_NUM_MAX				25 // 20 is orginal and +5 for over flow
#define ADC_UNIT							&hadc1
//current gain and conversion
#define R_SHUNT								0.001f
#define MAX_CURRENT						12.0f
#define G_AMP          				10U
#define G_ISO          				4U
#define G_TOTAL          			(G_AMP * G_ISO)
#define NOISE_THRESHOLD_LSB   3U
#endif// __ADC_UTILS_CONFIG__