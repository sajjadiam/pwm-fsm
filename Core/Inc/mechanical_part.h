#ifndef __MECHANICAL_PART__
#define __MECHANICAL_PART__

#include "stm32f1xx_hal.h"

typedef enum{
	MECHANICAL_PART_Relay		= 0,
	MECHANICAL_PART_Fan1,
	MECHANICAL_PART_InputControl,
	MECHANICAL_PARTS_END
}MECHANICAL_PARTS;

typedef struct{
	GPIO_TypeDef* GPIOPort;   /**< GPIO port, e.g., GPIOA */
	uint16_t GPIOPin;
	volatile GPIO_PinState	state;
}Mechanical_Part;

void Mechnical_Part_Init(Mechanical_Part* mp,GPIO_TypeDef* GPIOPort,uint16_t GPIOPin,GPIO_PinState	state);
void Mechnical_Part_Handler(Mechanical_Part* mechanicalPart ,GPIO_PinState mode);

#endif //__MECHANICAL_PART__