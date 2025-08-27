#include "mechanical_part.h"
#include "main.h"

static Mechanical_Part* s_mp;
void MP_AttachContext(Mechanical_Part* mp){
	s_mp = mp;
}

//---------------------------------------
void Mechnical_Part_Init(Mechanical_Part* mp,GPIO_TypeDef* GPIOPort,uint16_t GPIOPin,GPIO_PinState	state){
	mp->GPIOPort 	= GPIOPort;
	mp->GPIOPin 	= GPIOPin;
	mp->state 		= state;
}
void Mechnical_Parts_Init(Mechanical_Part* mp){
	Mechnical_Part_Init(&mp[MECHANICAL_PART_Relay],RELAY_GPIO_Port,RELAY_Pin,GPIO_PIN_RESET);
	Mechnical_Part_Init(&mp[MECHANICAL_PART_Fan1],FAN1_GPIO_Port,FAN1_Pin,GPIO_PIN_RESET);
	Mechnical_Part_Init(&mp[MECHANICAL_PART_InputControl],INPUT_CONTROLL_GPIO_Port,INPUT_CONTROLL_Pin,GPIO_PIN_RESET);
}
//--------------------------------------
void Mechnical_Part_Handler(Mechanical_Part* mechanicalPart ,GPIO_PinState mode){
	mechanicalPart->state = mode;
	HAL_GPIO_WritePin(mechanicalPart->GPIOPort,mechanicalPart->GPIOPin,(GPIO_PinState) mechanicalPart->state);
}
//end of mechnical_part.c