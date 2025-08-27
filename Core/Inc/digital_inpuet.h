#ifndef __DIGITAL_INPUT__
#define __DIGITAL_INPUT__
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"  /* Replace with your MCU family header if needed */
//------------------------------

typedef struct{
	GPIO_TypeDef* 					GPIOPort;   /**< GPIO port, e.g., GPIOA */
	uint16_t 								GPIOPin;    /**< GPIO pin, e.g., GPIO_PIN_0 */
	volatile GPIO_PinState	state;			/**< Pin state e.g.*/
}DI_PinConfig;

void DI_AttachContext(DI_PinConfig* d_input);
void DI_Init(DI_PinConfig* di);
void DI_read(DI_PinConfig* di);
#endif //__DIGITAL_INPUT__