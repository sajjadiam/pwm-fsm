#include "adc_utils.h"
#include "stm32f1xx_hal_adc.h"
#include <math.h>

extern ADC_HandleTypeDef hadc1;
volatile bool adc_dma_done = 0;
volatile bool adc_inject_done = 0;
uint16_t adc_dma_buffer[ADC_DMA_CHANNEL_COUNT];
uint32_t InjectTrigger = 0;											//Inject mode Trigger buffer
uint16_t currentSample[SAMPLE_NUM] = {0};				//adc sample buffer
uint16_t currentSampleCounter = 0;							//counter of sample number
uint32_t currentOffset = 0; 										//ADC no-load value or ADC offset
ADC_Current_Calibrate_Mode calibrateMode = ADC_Current_Calibrate_Mode_GetTrig; //clibrate mode handler
adc_funk calbrateurrentOffset_machine[ADC_Current_Calibrate_Mode_End] = {
	ADC_GetTrig			,
	ADC_SetTrig			,
	ADC_Start				,
	ADC_Sampling		,
	ADC_Processing	,
	ADC_ResetTrig		,
	ADC_Finishing		,
};
// ISR callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == hadc1.Instance) {
		adc_dma_done = true;
	}
}
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc){
	if(hadc->Instance == ADC1) {
		adc_inject_done = true;
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
//		value -= CURRENT_OFFSET;
		double Vsh = value / OPAMP_GAIN;
		float current = Vsh * SHUNT_RESISTOR_VALUE;
		return current;
	}
	return 0;
}
//----------------------------
uint32_t get_ADC_inject_trigger(ADC_HandleTypeDef* hadc){
	return hadc->Instance->CR2 & ADC_CR2_JEXTSEL;
}
HAL_StatusTypeDef ADC_inject_set_trigger(ADC_HandleTypeDef* hadc, uint32_t injectTrigger){
	HAL_ADCEx_InjectedStop_IT(hadc);
	HAL_ADC_Stop_DMA(hadc);        
  HAL_ADC_Stop(hadc);
	MODIFY_REG(hadc->Instance->CR2, ADC_CR2_JEXTSEL, injectTrigger);
	if((hadc->Instance->CR2 & ADC_CR2_JEXTSEL) == injectTrigger){
		return HAL_OK;
	}
	return HAL_ERROR;
}
uint16_t measure_adc_zero(void){
	
	return 0;
}
HAL_StatusTypeDef ADC_currentChannelCalibrate(void){
	calbrateurrentOffset_machine[calibrateMode]();
	if(calibrateMode == ADC_Current_Calibrate_Mode_Finishing){
		return HAL_OK;
	}
	return HAL_ERROR;
}
void ADC_GetTrig		(void){
	InjectTrigger = get_ADC_inject_trigger(ADC_CURRENT_UNIT);
	calibrateMode = ADC_Current_Calibrate_Mode_SetTrig;
	return;
}
void ADC_SetTrig		(void){
	if(ADC_inject_set_trigger(ADC_CURRENT_UNIT ,ADC_INJECTED_SOFTWARE_START) != HAL_OK){
		//set error code
		return;
	}
	calibrateMode = ADC_Current_Calibrate_Mode_Start;
	return;
}
void ADC_Start			(void){
	if(HAL_ADCEx_InjectedStart_IT(ADC_CURRENT_UNIT) != HAL_OK){
		//set error code
		return;
	}
	calibrateMode = ADC_Current_Calibrate_Mode_Sampling;
	return;
}
void ADC_Sampling		(void){
	if(adc_inject_done && currentSampleCounter < SAMPLE_NUM){
		adc_inject_done = false;
		currentSample[currentSampleCounter] = HAL_ADCEx_InjectedGetValue(ADC_CURRENT_UNIT, ADC_INJECTED_RANK_1);
		currentSampleCounter++;
		return;
	}
	else if(currentSampleCounter >= SAMPLE_NUM){
		if(HAL_ADCEx_InjectedStop_IT(ADC_CURRENT_UNIT) != HAL_OK){
			//set error code
			return;
		}
		currentSampleCounter = 0;
		calibrateMode = ADC_Current_Calibrate_Mode_Processing;
		return;
	}
}
void ADC_Processing	(void){
	uint32_t sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += currentSample[i];
	}
	currentOffset = sampleSum / SAMPLE_NUM;
	calibrateMode = ADC_Current_Calibrate_Mode_ResetTrig;
	return;
}
void ADC_ResetTrig	(void){
	if(ADC_inject_set_trigger(ADC_CURRENT_UNIT ,InjectTrigger) != HAL_OK){
		//set error code
		return;
	}
	calibrateMode = ADC_Current_Calibrate_Mode_Finishing;
	return;
}
void ADC_Finishing	(void){
	//do noting
}
