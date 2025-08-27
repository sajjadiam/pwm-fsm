#include "app_ctx.h"
#include <math.h>
#include <string.h>
#include "adc.h"

uint32_t InjectTrigger = 0;											//Inject mode Trigger buffer
static ADC_Context* s_ctx;

void ADC_AttachContext(ADC_Context* ctx){ 
	s_ctx = ctx; 
}

void ADC_Context_init(ADC_Context* ctx){
	ctx->adc = ADC_UNIT;
	memset(ctx->dma_buffer 		, 0 , sizeof(ctx->dma_buffer)		);
	memset(ctx->currentSample	, 0 , sizeof(ctx->currentSample));
	memset(ctx->voltageSample , 0 , sizeof(ctx->voltageSample));
	memset(ctx->temp1Sample 	, 0 , sizeof(ctx->temp1Sample)	);
	memset(ctx->temp2Sample 	, 0 , sizeof(ctx->temp2Sample)	);
	memset(ctx->sampleMean 		, 0 , sizeof(ctx->sampleMean)		);
	ctx->currentOffset 				= 0;
	ctx->currentSampleCounter = 0;
	ctx->dmaSampleCounter			= 0;
	ctx->oc_hits							= 0;
	ctx->dmaSampleReady				= 0;
	ctx->currentSampleReady		= 0;
}
bool manual_ADC_Enable(ADC_Context* ctx) {
	if(HAL_ADCEx_Calibration_Start(ctx->adc) != HAL_OK){
		return false;
	}
	if(HAL_ADC_Start_DMA(ctx->adc, (uint32_t*)ctx->dma_buffer, DMA_Index_End) != HAL_OK) {
		HAL_ADC_Stop_DMA(ctx->adc);
		return false;
	}
	if (HAL_ADCEx_InjectedStart_IT(ctx->adc) != HAL_OK) {
		HAL_ADCEx_InjectedStop_IT(ctx->adc);
		HAL_ADC_Stop_DMA(ctx->adc);
		return false;
	}
	return true;
}
bool manual_ADC_Disable(ADC_Context* ctx){
	// Stop DMA
	if (HAL_ADC_Stop_DMA(ctx->adc) != HAL_OK){
		return false;
	}
	// Stop Injected Conversion interrupts
	if (HAL_ADCEx_InjectedStop_IT(ctx->adc) != HAL_OK){
		return false;
	}
	return true;
}
CurrentCalibrateSub calibrateMode = CC_GetTrig; //clibrate mode handler
adc_funk calibrateCurrentOffset_machine[CC_End] = {
	[CC_GetTrig]					=	CC_Func_GetTrig			,
	[CC_SetTrig]					=	CC_Func_SetTrig			,
	[CC_Start]						=	CC_Func_Start				,
	[CC_Sampling]					= CC_Func_Sampling		,
	[CC_Processing]				= CC_Func_Processing	,
	[CC_Measuringccuracy] = CC_Func_Measuringccuracy	,
	[CC_ResetTrig]				= CC_Func_ResetTrig		,
	[CC_SetAWD]						= CC_Func_SetAWD			,
	[CC_Finishing]				= CC_Func_Finishing		,
};
// ISR callback
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == hadc1.Instance){
		ADC_Context* ctx = s_ctx;
		uint16_t* samples[DMA_Index_End] = {ctx->voltageSample, ctx->temp1Sample, ctx->temp2Sample};
		if(ctx->dmaSampleCounter < SAMPLE_NUM){
			for(DMA_Index i = DMA_Index_Vbus ;i < DMA_Index_End;i++){
				samples[i][ctx->dmaSampleCounter] = ctx->dma_buffer[i];
			}
			ctx->dmaSampleCounter++;
		}
	}
}
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc){
	if(hadc->Instance == ADC1) {
		ADC_Context* ctx = s_ctx;
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
	ADC_Context* ctx = s_ctx;
	uint16_t code = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1);
	if (code > (uint16_t)hadc->Instance->HTR){
		if (++ctx->oc_hits >= 3) {          // 3 برخورد پیاپی ⇒ HardStop
			//HardStop_Inverter();       // قطع PWM، اعلام Fault
			ctx->oc_hits = 0;
		}
	} 
	else if (code + OC_HYSTERESIS < (uint16_t)(hadc->Instance->HTR & 0x0FFF)){
		// اگر فاصله‌ای از High داریم (هیسترزیس)، شمارنده را صفر کن
		ctx->oc_hits = 0;
	}
}

// 3) ترکیب برای manual read once


void DC_Voltage_Safety_Checker(ADC_Context* ctx){
	float voltage = ADC_to_voltage(ctx->sampleMean[ADC_Channel_Voltage]);
	if			(voltage < 250){
		ctx->flags.underVoltage = 1;
	}
	else if	(voltage > 345){
		ctx->flags.overVoltage 	= 1;
	}
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
void Temperture_Safety_Checker(ADC_Context* ctx){
	float temperture1 = ADC_to_temperture(ctx->sampleMean[ADC_Channel_Temp1]);
	float temperture2 = ADC_to_temperture(ctx->sampleMean[ADC_Channel_Temp2]);
	if(temperture1 > 75 || temperture2 > 75){
		ctx->flags.overTemperture = 1;
	}
}
float ADC_to_current(uint16_t adc){
	ADC_Context* ctx = s_ctx;
	if(adc != 0){
		uint16_t adcRealVal = (adc - ctx->currentOffset + 0.5f);
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

void CC_Func_GetTrig					(ADC_Context* ctx){
	InjectTrigger = get_ADC_inject_trigger(ctx->adc);
	calibrateMode = CC_SetTrig;
	return;
}
void CC_Func_SetTrig					(ADC_Context* ctx){
	if(ADC_inject_set_trigger(ctx->adc ,ADC_INJECTED_SOFTWARE_START) != HAL_OK){
		//set error code
		return;
	}
	calibrateMode = CC_Start;
	return;
}
void CC_Func_Start						(ADC_Context* ctx){
	if(HAL_ADCEx_InjectedStart_IT(ctx->adc) != HAL_OK){
		//set error code
		return;
	}
	calibrateMode = CC_Sampling;
	return;
}
void CC_Func_Sampling					(ADC_Context* ctx){
	if(ctx->currentSampleCounter >= SAMPLE_NUM){
		if(HAL_ADCEx_InjectedStop_IT(ctx->adc) != HAL_OK){
			//set error code
			return;
		}
		ctx->currentSampleCounter = 0;
		calibrateMode = CC_Processing;
		return;
	}
	else{
		if(HAL_ADCEx_InjectedStart_IT(ctx->adc) != HAL_OK){
			//set error code
			return;
		}
	}
}
void CC_Func_Processing				(ADC_Context* ctx){
	uint32_t sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += ctx->currentSample[i];
	}
	ctx->currentOffset = sampleSum / SAMPLE_NUM + 0.5f;
	calibrateMode = CC_Measuringccuracy;
	return;
}
void CC_Func_Measuringccuracy	(ADC_Context* ctx){
	uint16_t deviation = 0;
	uint16_t deviationSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		deviation = ctx->currentSample[i] - ctx->currentOffset;
		deviationSum += (deviation * deviation);
	}
	double variance = deviationSum / (SAMPLE_NUM - 1);
	double standardDeviation = sqrt(variance);
	if(standardDeviation <= NOISE_THRESHOLD_LSB){
		calibrateMode = CC_ResetTrig;
		return;
	}
	else{
		calibrateMode = CC_Start;
		return;
	}
}
void CC_Func_ResetTrig				(ADC_Context* ctx){
	if(ADC_inject_set_trigger(ctx->adc ,InjectTrigger) != HAL_OK){
		//set error code
		return;
	}
	calibrateMode = CC_SetAWD;
	return;
}
void CC_Func_SetAWD						(ADC_Context* ctx){
	uint16_t awdHigh = CurrentA_to_ADCcounts(MAX_CURRENT) + ctx->currentOffset + (4 * NOISE_THRESHOLD_LSB);
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
	if((ctx->adc)->Lock != HAL_LOCKED){
		(ctx->adc)->Lock = HAL_LOCKED;
		(ctx->adc)->Instance->HTR = (uint32_t)awdHigh;
		__HAL_UNLOCK(ADC_UNIT);
	}
	else{
		return;
	}
	calibrateMode = CC_Finishing;
	return;
}
void CC_Func_Finishing				(ADC_Context* ctx){
	ctx->flags.calibrateDone = 1;
}
//end current calibrate offset
void DMA_Sampling				(ADC_Context* ctx){
	if(ctx->dmaSampleCounter >= SAMPLE_NUM){
		ctx->dmaSampleReady = 1;
		return;
	}
}
void INJECT_Sampling		(ADC_Context* ctx){
	if(ctx->currentSampleCounter >= SAMPLE_NUM){
		ctx->currentSampleReady = 1;
		return;
	}
}
void DMA_Processing			(ADC_Context* ctx){
	uint32_t sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += ctx->voltageSample[i];
	}
	ctx->sampleMean[ADC_Channel_Voltage] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
	sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += ctx->temp1Sample[i];
	}
	ctx->sampleMean[ADC_Channel_Temp1] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
	sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += ctx->temp2Sample[i];
	}
	ctx->sampleMean[ADC_Channel_Temp2] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
}
void INJECT_Processing	(ADC_Context* ctx){
	uint32_t sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += ctx->currentSample[i];
	}
	ctx->sampleMean[ADC_Channel_Curent] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
}
void ADC_Processing			(ADC_Context* ctx){
	int32_t sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += ctx->currentSample[i];
	}
	ctx->sampleMean[ADC_Channel_Curent] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
	sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += ctx->voltageSample[i];
	}
	ctx->sampleMean[ADC_Channel_Voltage] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
	sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += ctx->temp1Sample[i];
	}
	ctx->sampleMean[ADC_Channel_Temp1] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
	sampleSum = 0;
	for(uint16_t i = 0;i < SAMPLE_NUM;i++){
		sampleSum += ctx->temp2Sample[i];
	}
	ctx->sampleMean[ADC_Channel_Temp2] = (uint16_t)((sampleSum / SAMPLE_NUM) + 0.5f);
}
void safatyCheck				(ADC_Context* ctx){
	DC_Voltage_Safety_Checker(ctx);
	Temperture_Safety_Checker(ctx);
}
void calibratingCurrent	(ADC_Context* ctx){
	calibrateCurrentOffset_machine[calibrateMode](s_ctx);
}
void adcDisable					(ADC_Context* ctx){
	if(manual_ADC_Disable(ctx) == true){
		
	}
}
//end of adc_utils.c