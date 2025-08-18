#include "value_to_string.h"
#include <stdio.h>
#include "pwm.h"

SEVEN_SEGMENT_MODE segmentMode = SEVEN_SEGMENT_MODE_Off;
const SevenSegmentModeHandler sevenSegmentModeHandler[SEVEN_SEGMENT_MODE_END] = {
	[SEVEN_SEGMENT_MODE_Temperture]	= SevenSeg_TempertureConvertToString,
	[SEVEN_SEGMENT_MODE_Power]			= Sevenseg_PowerConvertToString,
	[SEVEN_SEGMENT_MODE_Frequency]	= Sevenseg_FrequencyConvertToString,
	[SEVEN_SEGMENT_MODE_Current]		= Sevenseg_CurrentConvertToString,
	[SEVEN_SEGMENT_MODE_Voltage]		= Sevenseg_VoltageConvertToString,
	[SEVEN_SEGMENT_MODE_State]			= Sevenseg_StateConvertToString,
	[SEVEN_SEGMENT_MODE_Error]			= Sevenseg_ErrorConvertToString,
	[SEVEN_SEGMENT_MODE_Time]				= Sevenseg_TimeConvertToString,
	[SEVEN_SEGMENT_MODE_Init]				= SevenSeg_InitMode,
	[SEVEN_SEGMENT_MODE_Off]				= SevenSeg_OffMode,
};
//convert Temperture value of float to string for use in seven segment
void SevenSeg_TempertureConvertToString(char* str ,void* P){
	PWM_State_t* pwm = (PWM_State_t*)P;
	float temp = pwm->heatSinkTemp;
	if(temp <= -10.0){
		snprintf(str, 5, "%.0fC",temp);
	}
	if(temp < 0.0 && temp > -10.0){
		snprintf(str, 6, "%.1fC",temp);
	}
	else if(temp < 10.0 && temp >= 0){
		snprintf(str, 6, "%.2fC",temp);
	}
	else if(temp < 100.0 && temp >= 10.0){
		snprintf(str, 6, "%.1fC",temp);
	}
	else if(temp >= 100.0){
		snprintf(str, 5,"%.0fC",temp);
	}
}
//convert Power value of float to string for use in seven segment
void Sevenseg_PowerConvertToString(char* str ,void* P){
	PWM_State_t* pwm = (PWM_State_t*)P;
	float power = (pwm->voltage * pwm->current) / 1000;
	snprintf(str ,6,"%1.2fP",power);//power in x.xx KW
}
//convert Frequency value of float to string for use in seven segment
void Sevenseg_FrequencyConvertToString(char* str ,void* P){
	PWM_State_t* pwm = (PWM_State_t*)P;
  snprintf(str ,6,"%2.1fH",((float)pwm->currentFreq / 1000)); //frequency in xx.x KHz
}
//convert Current value of float to string for use in seven segment
void Sevenseg_CurrentConvertToString(char* str ,void* P){
	PWM_State_t* pwm = (PWM_State_t*)P;
	float i = pwm->current;
	if(i >= 0 && i < 10.0){
		snprintf(str ,6,"%1.2fA",i);//current in x.xx A
	}
	else if(i >= 10.0){
		snprintf(str ,6,"%2.1fA",i);//current in xx.x A
	}
}
//convert Voltage value of float to string for use in seven segment
void Sevenseg_VoltageConvertToString(char* str ,void* P){
	PWM_State_t* pwm = (PWM_State_t*)P;
	snprintf(str ,6,"%3.0fU",pwm->voltage);//voltage in xxx V
}
//convert State value of float to string for use in seven segment
void Sevenseg_StateConvertToString(char* str ,void* P){
	PWM_State_t* pwm = (PWM_State_t*)P;
	snprintf(str ,5,"St%02d",pwm->currentState);
}
//convert Error value of float to string for use in seven segment
void Sevenseg_ErrorConvertToString(char* str ,void* P){
	PWM_State_t* pwm = (PWM_State_t*)P;
	ERROR_CODE code = pwm->errorCode;
	if(code < ERROR_CODE_Evt_Start){
		snprintf(str ,5,"Err%01d",code);
	}
	if(code >= ERROR_CODE_Evt_Start){
		snprintf(str ,5,"Er%02d",code);
	}
}
void Sevenseg_TimeConvertToString(char* str ,void* P){
	PWM_State_t* pwm = (PWM_State_t*)P;
	TIME time = pwm->time;;
	snprintf(str ,6 ,"%02d.%02d" ,time.minute ,time.second);
}
// init mode
void SevenSeg_InitMode(char* str ,void* P){
	// this function only for inteing wait for initial values 
	
}
void SevenSeg_OffMode(char* str ,void* P){
	
}