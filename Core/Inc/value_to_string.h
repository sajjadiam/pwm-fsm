#ifndef __VALUE_TO_STRING__
#define __VALUE_TO_STRING__

#include <stdint.h>

typedef enum{
	SEVEN_SEGMENT_MODE_Temperture	=0,
	SEVEN_SEGMENT_MODE_Power,
	SEVEN_SEGMENT_MODE_Frequency,
	SEVEN_SEGMENT_MODE_Current,
	SEVEN_SEGMENT_MODE_Voltage,
	SEVEN_SEGMENT_MODE_State,
	SEVEN_SEGMENT_MODE_Error,
	SEVEN_SEGMENT_MODE_Time,
	SEVEN_SEGMENT_MODE_Init,
	SEVEN_SEGMENT_MODE_Off,
	SEVEN_SEGMENT_MODE_END,
}SEVEN_SEGMENT_MODE;
typedef void (*SevenSegmentModeHandler)(char* str ,void* P);

extern SEVEN_SEGMENT_MODE segmentMode;
extern const SevenSegmentModeHandler sevenSegmentModeHandler[SEVEN_SEGMENT_MODE_END];

void SevenSeg_TempertureConvertToString(char* str ,void* P);			// تبديل مقدار دما به رشته براي سون سگمنت 
void Sevenseg_PowerConvertToString(char* str,void* P);			// تبديل مقدار توان به رشته براي سون سگمنت 
void Sevenseg_FrequencyConvertToString(char* str,void* p);		// تبديل مقدار فرکانس به رشته براي سون سگمنت 
void Sevenseg_CurrentConvertToString(char* str,void* p);						// تبديل مقدار جريان به رشته براي سون سگمنت 
void Sevenseg_VoltageConvertToString(char* str,void* p);						// تبديل مقدار ولتاژ به رشته براي سون سگمنت 
void Sevenseg_StateConvertToString(char* str,void* p);				// تبديل مقدار حالت ماشين به رشته براي سون سگمنت 
void Sevenseg_ErrorConvertToString(char* str,void* p);				// تبديل مقدار ارور به رشته براي سون سگمنت 
void Sevenseg_TimeConvertToString(char* str,void* p);	// تبديل مقدار زمان به رشته براي سون سگمنت 
void SevenSeg_InitMode(char* str,void* p);																	// تبديل مقدار تايمر شروع اوليه براي مقدار اوليه به رشته براي سون سگمنت 
void SevenSeg_OffMode(char* str,void* p);
#endif //__VALUE_TO_STRING__