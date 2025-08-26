#ifndef __KEY_H__
#define __KEY_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"  /* Replace with your MCU family header if needed */
/* Exported types ------------------------------------------------------------*/
typedef enum{
	PWM_COMMMAND_KEY	= 0,
	SCREEN_MODE_KEY		= 1,
	END_KEY,
}KEYS;
/**
 * @enum KeyState
 * @brief Enum representing the 2-bit key state.
 *
 * Possible states of the key:
 * - KeyStateOnHold: Key is being held down.
 * - KeyStateReleased: Key was just released.
 * - KeyStatePressed: Key was just pressed.
 * - KeyStateOnNone: No change (key remains unpressed).
 */
typedef enum{
	KeyStateOnHold    = 0,  /**< 0b00: Key is being held down */
	KeyStateReleased  = 1,  /**< 0b01: Key was just released */
	KeyStatePressed   = 2,  /**< 0b10: Key was just pressed */
	KeyStateOnNone    = 3   /**< 0b11: No change (key remains unpressed) */
}KeyState;
/**
 * @typedef KeyCallback
 * @brief Callback function type for key events.
 * 
 * @param callbakArg Pointer to argument, usage depends on key event context.
 */
//typedef void (*KeyCallback)(KeyPinConfig* key);
/**
 * @struct KeyPinConfig
 * @brief Structure representing a key pin configuration.
 *
 * Holds the GPIO port and pin, key state, and callback handler.
 */
typedef struct{
	GPIO_TypeDef* GPIOPort;   /**< GPIO port, e.g., GPIOA */
	uint16_t GPIOPin;         /**< GPIO pin, e.g., GPIO_PIN_0 */
	volatile KeyState state;  /**< Internal state for 2-bit tracking */
}KeyPinConfig;
/* Exported functions --------------------------------------------------------*/
/**
 * @brief Initialize key configurations.
 *
 * @param key Pointer to an array of KeyPinConfig structs.
 * @param len Number of elements in the key array.
 */
void Key_Init(KeyPinConfig* key, uint16_t len);
/**
 * @brief Read and update key states.
 *
 * Should be called periodically (e.g., every 30ms) to update and read key states.
 *
 * @param key Pointer to an array of KeyPinConfig structs.
 * @param len Number of elements in the key array.
 */
void Key_Read(KeyPinConfig* key, uint16_t len);
#ifdef __cplusplus
}
#endif
#endif /* __KEY_H__ */