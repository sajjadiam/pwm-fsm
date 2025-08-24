#include "adc_utils.h"
#include <math.h>
#include <string.h>
#include "pwm.h"
#include "adc.h"

uint32_t InjectTrigger = 0;											//Inject mode Trigger buffer


void ADC_Context_init(ADC_Context* ctx){
	memset(ctx->dma_buffer 		, 0 , sizeof(ctx->dma_buffer)		);
	memset(ctx->currentSample	, 0 , sizeof(ctx->currentSample));
	memset(ctx->voltageSample , 0 , sizeof(ctx->voltageSample));
	memset(ctx->temp1Sample 	, 0 , sizeof(ctx->temp1Sample)	);
	memset(ctx->temp2Sample 	, 0 , sizeof(ctx->temp2Sample)	);
	memset(ctx->sampleMean 		, 0 , sizeof(ctx->sampleMean)		);
	ctx->currentOffset 				= 0;
	ctx->currentSampleCounter = 0;
	ctx->dmaSampleCounter			= 0;
	ctx->dmaSampleReady				= 0;
	ctx->currentSampleReady		= 0;
}
bool manual_ADC_Enable(ADC_Context* ctx) {
	if(HAL_ADCEx_Calibration_Start(ADC_UNIT) != HAL_OK){
		return false;
	}
	if(HAL_ADC_Start_DMA(ADC_UNIT, (uint32_t*)ctx->dma_buffer, DMA_Index_End) != HAL_OK) {
		HAL_ADC_Stop_DMA(ADC_UNIT);
		return false;
	}
	if (HAL_ADCEx_InjectedStart_IT(ADC_UNIT) != HAL_OK) {
		HAL_ADCEx_InjectedStop_IT(ADC_UNIT);
		HAL_ADC_Stop_DMA(ADC_UNIT);
		return false;
	}
	return true;
}
bool manual_ADC_Disable(ADC_Context* ctx){
	// Stop DMA
	if (HAL_ADC_Stop_DMA(ADC_UNIT) != HAL_OK){
		return false;
	}
	// Stop Injected Conversion interrupts
	if (HAL_ADCEx_InjectedStop_IT(ADC_UNIT) != HAL_OK){
		return false;
	}
	return true;
}
CurrentCalibrateSub calibrateMode = CC_GetTrig; //clibrate mode handler
adc_funk calibrateCurrentOffset_machine[CC_End] = {
	[CC_GetTrig]					=	ADC_GetTrig			,
	[CC_SetTrig]					=	ADC_SetTrig			,
	[CC_Start]						=	ADC_Start				,
	[CC_Sampling]					= ADC_Sampling		,
	[CC_Processing]				= ADC_Processing	,
	[CC_Measuringccuracy] = ADC_Measuringccuracy	,
	[CC_ResetTrig]				= ADC_ResetTrig		,
	[CC_Finishing]				= ADC_Finishing		,
	[CC_SetAWD]						= ADC_SetAWD			,
};
// ISR callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == hadc1.Instance){
		ADC_Context* ctx = &adcCtx;
		uint16_t* samples[] = {ctx->voltageSample, ctx->temp1Sample, ctx->temp2Sample};
		if(ctx->dmaSampleCounter < SAMPLE_NUM){
			for(DMA_Index i = DMA_Index_Vbus ;i < DMA_Index_End;i++){
				samples[i][ctx->dmaSampleCounter] = ctx->dma_buffer;
			}
			ctx->dmaSampleCounter++;
		}
	}
}
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc){
	if(hadc->Instance == ADC1) {
		ADC_Context* ctx = &adcCtx;
		if(ctx->currentSampleCounter < SAMPLE_NUM){
			ctx->currentSample[ctx->currentSampleCounter] = HAL_ADCEx_InjectedGetValue(ADC_UNIT, ADC_INJECTED_RANK_1);
			ctx->currentSampleCounter++;
		}
	}
}

/* واچ‌داگ: اگر مقدار از بازه‌ی تعیین‌شده خارج شد */
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc){
	if(hadc->Instance != ADC1){
		return;
	}
	if(hadc->Instance == ADC1){
		/* اقدام حفاظت: مثلاً خاموش کردن PWM یا اعلام خطا */
	}
}

// 3) ترکیب برای manual read once


bool DC_Voltage_Safety_Checker(void){
	float voltage = ADC_to_voltage(dmaSampleMean[ADC_IDX_VBUS]);
	if(voltage < 250 || voltage > 345){
		return false;
	}
	return true;
}
float ADC_to_voltage(uint16_t adc){
	if(adc != 0){
		float voltage = ADCcounts_to_VoltageV(adc);
		return voltage;
	}
	return 0;
}
float ADC_to_temperture(uint16_t adc){
	if(adc != 0){
		float R_ntc = R_FIXED * ((float)adc) / (ADC_MAX - (float)adc);
		float invT = (1.0f / T0) + (1.0f / B) * logf(R_ntc / R0);
    float temp_c = (1.0f / invT) - 273.15f;
		return temp_c;
	}
	return 0;
}
bool Temperture_Safety_Checker(void){
	float temperture1 = ADC_to_temperture(dmaSampleMean[ADC_IDX_TEMP_CH1]);
	float temperture2 = ADC_to_temperture(dmaSampleMean[ADC_IDX_TEMP_CH2]);
	if(temperture1 > 75 || temperture2 > 75){
		return false;
	}
	return true;
}
float ADC_to_current(uint16_t adc){
	if(adc != 0){
		uint16_t adcRealVal = (adc - currentOffset + 0.5f);
		float current = ADCcounts_to_CurrentA(adcRealVal);
		return current;
	}
	return 0;
}
//start current calibrate offset
uint32_t get_ADC_inject_trigger(ADC_HandleTypeDef* hadc){
	return (hadc->Instance->CR2 & ADC_CR2_JEXTSEL);
}
HAL_StatusTypeDef ADC_inject_set_trigger(ADC_HandleTypeDef* hadc, uint32_t injectTrigger){
	HAL_ADCEx_InjectedStop_IT(hadc);
	HAL_ADC_Stop_DMA(hadc);        
  HAL_ADC_Stop(hadc);
	MODIFY_REG(hadc->Instance->CR2, ADC_CR2_JEXTSEL, injectTrigger);
	if((hadc->Instance->CR2 & ADC_CR2_JEXTSEL) != injectTrigger){
		return HAL_ERROR;
	}
	if (injectTrigger == ADC_INJECTED_SOFTWARE_START){
		// تریگر خارجی را خاموش کن؛ HAL در Start_IT برای SW خودش JSWSTART را می‌زند
		CLEAR_BIT(hadc->Instance->CR2, ADC_CR2_JEXTTRIG);

		// 4) شروع (یک‌شات) نرم‌افزاری با وقفه
		//    (اگر وقفه نمی‌خواهی، می‌توانی از HAL_ADCEx_InjectedStart استفاده کنی)
		return HAL_OK;
	}
	else{
		// منبع خارجی است: اجازهٔ تریگر خارجی Injected را روشن کن
		SET_BIT(hadc->Instance->CR2, ADC_CR2_JEXTTRIG);

		// 4) آرم‌کردن گروه Injected: از این لحظه با هر TRGO از تایمر شروع می‌شود
		return HAL_OK;
	}
	return HAL_ERROR;
}
bool ADC_currentChannelCalibrate(void){
	calibrateCurrentOffset_machine[calibrateMode]();
	if(calibrateMode == ADC_Current_Calibrate_Mode_Finishing){
		return true;
	}
	return false;
}
void ADC_GetTrig		(void){
	InjectTrigger = get_ADC_inject_trigger(ADC_UNIT);
	calibrateMode = ADC_Current_Calibrate_Mode_SetTrig;
	return;
}
void ADC_SetTrig		(void){
	if(ADC_inject_set_trigger(ADC_UNIT ,ADC_INJECTED_SOFTWARE_START) != HAL_OK){
		//set error code
		return;
	}
	calibrateMode = ADC_Current_Calibrate_Mode_Start;
	return;
}
void ADC_Start			(void){
	if(HAL_ADCEx_InjectedStart_IT(ADC_UNIT) != HAL_OK){
		//set error code
		return;
	}
	calibrateMode = ADC_Current_Calibrate_Mode_Sampling;
	return;
}
void ADC_Sampling		(void){
	INJECT_GET_SAMPLE();
	if(currentSampleCounter >= SampleNum[pwmState.currentState]){
		if(HAL_ADCEx_InjectedStop_IT(ADC_UNIT) != HAL_OK){
			//set error code
			return;
		}
		currentSampleCounter = 0;
		calibrateMode = ADC_Current_Calibrate_Mode_Processing;
		return;
	}
	else{
		if(HAL_ADCEx_InjectedStart_IT(ADC_UNIT) != HAL_OK){
			//set error code
			return;
		}
	}
}
void ADC_Processing	(void){
	uint32_t sampleSum = 0;
	for(uint16_t i = 0;i < SampleNum[pwmState.currentState];i++){
		sampleSum += currentSample[i];
	}
	currentOffset = sampleSum / SampleNum[pwmState.currentState] + 0.5f;
	calibrateMode = ADC_Current_Calibrate_Mode_Measuringccuracy;
	return;
}
void ADC_Measuringccuracy	(void){
	uint16_t deviation = 0;
	uint16_t deviationSum = 0;
	for(uint16_t i = 0;i < SampleNum[pwmState.currentState];i++){
		deviation = currentSample[i] - currentOffset;
		deviationSum += (deviation * deviation);
	}
	double variance = deviationSum / (SampleNum[pwmState.currentState] - 1);
	double standardDeviation = sqrt(variance);
	if(standardDeviation <= NOISE_THRESHOLD_LSB){
		calibrateMode = ADC_Current_Calibrate_Mode_ResetTrig;
		return;
	}
	else{
		calibrateMode = ADC_Current_Calibrate_Mode_Start;
		return;
	}
}
void ADC_ResetTrig	(void){
	if(ADC_inject_set_trigger(ADC_UNIT ,InjectTrigger) != HAL_OK){
		//set error code
		return;
	}
	calibrateMode = ADC_Current_Calibrate_Mode_SetAWD;
	return;
}
void ADC_SetAWD			(void){
	uint16_t awdHigh = CurrentA_to_ADCcounts(MAX_CURRENT) + currentOffset + (4 * NOISE_THRESHOLD_LSB);
	if(awdHigh <= 0){
		//error : calibrateMode faild -> check MAX_CURRENT and other values
		//calibrateMode = 
		return;
	}
  else if(awdHigh > 4095){
		//error : calibrateMode faild -> Reduce the gain. out of range.
		//PWM_FSM_HandleEvent(Evt_HardwareFault);
		return;
	}
	if((*ADC_UNIT).Lock != HAL_LOCKED){
		(ADC_UNIT)->Lock = HAL_LOCKED;
		(ADC_UNIT)->Instance->HTR = (uint32_t)awdHigh;
		__HAL_UNLOCK(ADC_UNIT);
	}
	else{
		return;
	}
	calibrateMode = ADC_Current_Calibrate_Mode_Finishing;
	return;
}
void ADC_Finishing	(void){
	//do noting
}
//end current calibrate offset
//----------------------------
void DMA_GET_SAMPLE(void){
	if(adc_dma_done){
		adc_dma_done = false;
		voltageSample[dmaSampleCounter] = adc_dma_buffer[ADC_IDX_VBUS];
		temp1Sample[dmaSampleCounter] = adc_dma_buffer[ADC_IDX_TEMP_CH1];
		temp2Sample[dmaSampleCounter] = adc_dma_buffer[ADC_IDX_TEMP_CH2];
		dmaSampleCounter++;
		if(dmaSampleCounter >= SampleNum[pwmState.currentState]){
			dmaSampleCounter = 0;
			dmaSampleReady = true;
			return;
		}
	}
}
void INJECT_GET_SAMPLE(void){
	if(adc_inject_done){
		adc_inject_done = false;
		currentSample[currentSampleCounter] = HAL_ADCEx_InjectedGetValue(ADC_UNIT, ADC_INJECTED_RANK_1);
		currentSampleCounter++;
		if(currentSampleCounter >= SampleNum[pwmState.currentState]){
			currentSampleCounter = 0;
			currentSampleReady = true;
			return;
		}
	}
}
//end of adc_utils.c
