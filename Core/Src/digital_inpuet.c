#include "digital_inpuet.h"
#include "main.h"

static DI_PinConfig* s_di;

void DI_AttachContext(DI_PinConfig* d_input){
	s_di = d_input;
}
void DI_Init(DI_PinConfig* di){
	di->GPIOPort = ss_CONTROLL_GPIO_Port;
	di->GPIOPin = ss_CONTROLL_Pin;
	di->state = HAL_GPIO_ReadPin(di->GPIOPort,di->GPIOPin);
}
void DI_read(DI_PinConfig* di){
	di->state = HAL_GPIO_ReadPin(di->GPIOPort,di->GPIOPin);
}
//end of digital_inpuet.c