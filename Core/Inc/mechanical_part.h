#ifndef __MECHANICAL_PART__
#define __MECHANICAL_PART__

#include "stm32f1xx_hal.h"

typedef enum{
	POWER_MODE_OFF	= 0,
	POWER_MODE_ON,
}POWER_MODE;
typedef enum{
	MECHANICAL_PART_Relay		= 0,
	MECHANICAL_PART_Fan1,
	MECHANICAL_PART_Fan2,
	MECHANICAL_PARTS_END
}MECHANICAL_PARTS;
typedef struct{
	GPIO_TypeDef* GPIOPort;   /**< GPIO port, e.g., GPIOA */
	uint16_t GPIOPin;
	POWER_MODE mode;
}Mechnical_Part;

extern Mechnical_Part mechParts[MECHANICAL_PARTS_END];
void Mechnical_Part_Init(Mechnical_Part* mechanicalPart ,uint16_t len);
void Mechnical_Part_Handler(Mechnical_Part* mechanicalPart ,POWER_MODE mode);

#endif //__MECHANICAL_PART__