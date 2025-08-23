#include "mechanical_part.h"
#include "main.h"

Mechanical_Part mechParts[MECHANICAL_PARTS_END] = {
	{RELAY_GPIO_Port 					,RELAY_Pin 					,POWER_MODE_OFF},
	{FAN1_GPIO_Port 					,FAN1_Pin 					,POWER_MODE_OFF },
	{INPUT_CONTROLL_GPIO_Port	,INPUT_CONTROLL_Pin	,POWER_MODE_OFF },
};
//---------------------------------------
void Mechnical_Part_Init(Mechanical_Part* mechanicalPart ,uint16_t len){
	while(len-- > 0){
		HAL_GPIO_WritePin(mechanicalPart->GPIOPort,mechanicalPart->GPIOPin,(GPIO_PinState) mechanicalPart->mode);
		mechanicalPart++;
	}
}
//--------------------------------------
void Mechnical_Part_Handler(Mechanical_Part* mechanicalPart ,POWER_MODE mode){
	mechanicalPart->mode = mode;
	HAL_GPIO_WritePin(mechanicalPart->GPIOPort,mechanicalPart->GPIOPin,(GPIO_PinState) mechanicalPart->mode);
}
//end of mechnical_part.c