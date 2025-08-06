#include "mechanical_part.h"
#include "main.h"

Mechnical_Part mechParts[MECHANICAL_PARTS_END] = {
	{RELAY_GPIO_Port ,RELAY_Pin ,POWER_MODE_OFF},
	{FAN1_GPIO_Port ,FAN1_Pin ,POWER_MODE_OFF },
	{FAN2_GPIO_Port ,FAN2_Pin ,POWER_MODE_OFF },
};

void Mechnical_Part_Init(Mechnical_Part* mechanicalPart ,uint16_t len){
	while(len-- > 0){
		HAL_GPIO_WritePin(mechanicalPart->GPIOPort,mechanicalPart->GPIOPin,(GPIO_PinState) mechanicalPart->mode);
		mechanicalPart++;
	}
}
void Mechnical_Part_Handler(Mechnical_Part* mechanicalPart ,POWER_MODE mode){
	mechanicalPart->mode = mode;
	HAL_GPIO_WritePin(mechanicalPart->GPIOPort,mechanicalPart->GPIOPin,(GPIO_PinState) mechanicalPart->mode);
}