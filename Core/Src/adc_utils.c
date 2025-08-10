#include "adc_utils.h"
#include "stm32f1xx_hal_adc.h"
#include <math.h>

extern ADC_HandleTypeDef hadc1;
volatile bool adc_dma_done;
uint16_t adc_dma_buffer[ADC_DMA_CHANNEL_COUNT];
uint16_t adc_current_buffer = 0;

// ISR callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == hadc1.Instance) {
		adc_dma_done = true;
	}
}
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc){
	if(hadc->Instance == ADC1) {
		adc_current_buffer = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
		/* تبدیل مقدار خام به جریان و ذخیره در متغیر یا ساختار دلخواه */
	}
}

/* واچ‌داگ: اگر مقدار از بازه‌ی تعیین‌شده خارج شد */
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc){
	if(hadc->Instance == ADC1) {
		/* اقدام حفاظت: مثلاً خاموش کردن PWM یا اعلام خطا */
	}
}

// 3) ترکیب برای manual read once

bool manual_ADC_Enable(void) {
	if(HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK){
		return false;
	}
	if(HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buffer, ADC_DMA_CHANNEL_COUNT) != HAL_OK) {
		HAL_ADC_Stop_DMA(&hadc1);
		return false;
	}
	if (HAL_ADCEx_InjectedStart_IT(&hadc1) != HAL_OK) {
		HAL_ADCEx_InjectedStop_IT(&hadc1);
		HAL_ADC_Stop_DMA(&hadc1);
		return false;
	}
	return true;
}
bool manual_ADC_Disable(void){
	// Stop DMA
	if (HAL_ADC_Stop_DMA(&hadc1) != HAL_OK){
		return false;
	}
	// Stop Injected Conversion interrupts
	if (HAL_ADCEx_InjectedStop_IT(&hadc1) != HAL_OK){
		return false;
	}
	return true;
}
bool DC_Voltage_Safety_Checker(void){
	float voltage = ADC_to_voltage(adc_dma_buffer[ADC_IDX_VBUS]);
	if(voltage < 250 || voltage > 345){
		return false;
	}
	return true;
}
float ADC_to_voltage(uint16_t adc){
	if(adc != 0){
		float value = ((float)adc / ADC_MAX) * V_REF;
		float voltage = value * VOLTAGE_GAIN;
		return voltage;
	}
	return 0;
}
float ADC_to_temperture(uint16_t adc){
	if(adc != 0){
		float V_adc = ((float)adc / ADC_MAX) * V_REF;
		float R_ntc = R_FIXED * ((V_REF / V_adc) - 1.0);
		float temp_kelvin = 1.0 / ((1.0 / T0) + (1.0 / B) * log(R_ntc / R0));
		float temp_celsius = temp_kelvin - 273.15;
		return temp_celsius;
	}
	return 0;
}
bool Temperture_Safety_Checker(void){
	float temperture1 = ADC_to_temperture(adc_dma_buffer[ADC_IDX_TEMP_CH1]);
	float temperture2 = ADC_to_temperture(adc_dma_buffer[ADC_IDX_TEMP_CH2]);
	if(temperture1 > 75 || temperture2 > 75){
		return false;
	}
	return true;
}
float ADC_to_current(uint16_t adc){
	if(adc != 0){
		float value = ((float)adc / ADC_MAX) * V_REF;
		float current = value * CURRENT_GAIN;
		return current;
	}
	return 0;
}