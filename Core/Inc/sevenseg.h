/* ===========================================================================
 * @file    sevenseg.h
 * @brief   Doxygen API header for 7‑segment display library.
 *          Provides inline controls, handle type and public functions.
 *
 * @author  Your Name
 * @date    2025‑07‑28
 * ===========================================================================
 */
#ifndef __SEVENSEG_H__
#define __SEVENSEG_H__

#ifdef __cplusplus
extern "C" {
#endif

/*──────────────────────────────────────────────────────────────────────────────*/
/** @defgroup Includes
 *  @{ */
#include <stdint.h>
#include <assert.h>
#include "sevenseg_pins.h"      /**< GPIO pin and port definitions */
#include "sevenseg_map.h"       /**< Segment bitmasks and ASCII map */

/** @} */

/*──────────────────────────────────────────────────────────────────────────────*/
/** @defgroup ControlInline  Inline Control Functions
 *  @{ */



/*──────────────────────────────────────────────────────────────────────────────*/
/** @defgroup Types  Public Types and Handle
 *  @{ */

/**
 * @struct SevenSeg_HandleTypeDef
 * @brief  Handle structure for a 4‑digit 7‑segment display.
 */
typedef struct {
#if (USE_COMMON_SEGMENT_PORT)
    GPIO_TypeDef*   segmentPort;                          /**< Common segment port */
#else
    GPIO_TypeDef*   segmentPorts[SEGMENT_NUM];            /**< Individual segment ports (A–DP) */
#endif

#if (USE_COMMON_DIGIT_PORT)
    GPIO_TypeDef*   digitPort;                            /**< Common digit port */
#else
    GPIO_TypeDef*   digitPorts[DIGIT_NUM];                /**< Individual digit ports (1–4) */
#endif

    uint16_t        segmentPins[SEGMENT_NUM];             /**< GPIO pins for segments */
    uint16_t        digitPins[DIGIT_NUM];                 /**< GPIO pins for digits */

    uint8_t         currentDigit;                     		/**< Active digit index (0..3) */
    uint8_t         buffer[DIGIT_NUM];                    /**< Digit buffer (0..9 or ASCII) */

} SevenSeg_HandleTypeDef;

/** @} end of Types */

/*──────────────────────────────────────────────────────────────────────────────*/
/** @defgroup PublicAPI  Public Function Prototypes
 *  @{ */

/**
 * @brief  Initialize the 7‑segment display handle.
 * @param  hseg  Pointer to a pre‑configured handle.
 */
void SevenSeg_Init(SevenSeg_HandleTypeDef* hseg);

/**
 * @brief  Refresh/update the display (to be called in timer ISR or loop).
 * @param  hseg  Pointer to the display handle.
 */
void SevenSeg_Update(SevenSeg_HandleTypeDef* hseg);

/**
 * @brief  Find the index of the decimal point in a string.
 * @param  str   Null‑terminated string.
 * @return Index of '.' if found, otherwise -1.
 */
int8_t SevenSeg_GetDotIndex(char* str);

/**
 * @brief  Load a text string into the digit buffer.
 * @param  hseg  Pointer to the display handle.
 * @param  str   Null‑terminated string (max length = DIGIT_NUM).
 */
void SevenSeg_BufferUpdate(SevenSeg_HandleTypeDef* hseg,char* str);

/** @} end of PublicAPI */
/**
 * @brief   Write raw pattern to all segments on a common port.
 * @param   port   GPIO port pointer.
 * @param   val    Unshifted bit pattern for segments.
 * @param   mask   Mask of all segment pins on that port.
 * @param   shift  Number of bits to shift `val` left.
 */
 #if (USE_COMMON_SEGMENT_PORT)
static inline void SevenSeg_SetSegments(GPIO_TypeDef *port, uint8_t val, uint16_t mask, uint16_t shift){
	assert(port != NULL);
	uint16_t v = (val << shift) & mask;
	if (SEGMENT_ACTIVE_LVL == SEGMENT_ACTIVE_HIGH) {
		port->ODR = (port->ODR & ~mask) | v;
	} 
	else {
		port->ODR = (port->ODR & ~mask) & ~v;
	}
}
/**
 * @brief   Clear all segment pins on a common port.
 * @param   port   GPIO port pointer.
 * @param   mask   Mask of all segment pins on that port.
 * @param   shift  (unused—to match signature).
 */
static inline void SevenSeg_ClrSegments(GPIO_TypeDef *port, uint16_t mask){
	assert(port != NULL);
	if (SEGMENT_ACTIVE_LVL == SEGMENT_ACTIVE_HIGH) {
		port->ODR &= ~mask;
	} 
	else {
		port->ODR |= mask;
	}
}

/**
 * @brief   Set a single segment pin.
 * @param   portPtr  GPIO port pointer.
 * @param   pin      GPIO pin number.
 */
static inline void SevenSeg_SetSeg(GPIO_TypeDef *portPtr, uint16_t pin){
	assert(portPtr != NULL);
	HAL_GPIO_WritePin(portPtr, pin, (SEGMENT_ACTIVE_LVL == SEGMENT_ACTIVE_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
#else
/**
 * @brief   Clear a single segment pin.
 * @param   portPtr  GPIO port pointer.
 * @param   pin      GPIO pin number.
 */
static inline void SevenSeg_ClrSeg(GPIO_TypeDef *portPtr, uint16_t pin){
	assert(portPtr != NULL);
	HAL_GPIO_WritePin(portPtr, pin, (SEGMENT_ACTIVE_LVL == SEGMENT_ACTIVE_HIGH) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}

/**
 * @brief   Write raw pattern to all digits on a common port.
 * @param   port   GPIO port pointer.
 * @param   val    One-hot bit pattern for digits.
 * @param   mask   Mask of all digit pins on that port.
 */
#endif

 #if (USE_COMMON_DIGIT_PORT)
static inline void SevenSeg_SetDigits(GPIO_TypeDef *port, uint16_t val, uint16_t mask){
	assert(port != NULL);
	uint16_t v = (val & mask);
	if (DIGIT_ACTIVE_LVL == DIGIT_ACTIVE_HIGH) {
		port->ODR = (port->ODR & ~mask) | v;
	} 
	else {
		port->ODR = (port->ODR & ~mask) & ~v;
	}
}

/**
 * @brief   Clear all digit pins on a common port.
 * @param   port   GPIO port pointer.
 * @param   mask   Mask of all digit pins on that port.
 */
static inline void SevenSeg_ClrDigits(GPIO_TypeDef *port, uint16_t mask){
	assert(port != NULL);
	if (DIGIT_ACTIVE_LVL == DIGIT_ACTIVE_HIGH) {
		port->ODR &= ~mask;
	} 
	else {
		port->ODR |= mask;
	}
}
#else
/**
 * @brief   Clear a single digit pin.
 * @param   portPtr  GPIO port pointer.
 * @param   pin      GPIO pin number.
 */
static inline void SevenSeg_ClrDig(GPIO_TypeDef *portPtr, uint16_t pin){
	assert(portPtr != NULL);
	HAL_GPIO_WritePin(portPtr, pin, (DIGIT_ACTIVE_LVL == DIGIT_ACTIVE_HIGH) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}


/**
 * @brief   Set a single digit pin.
 * @param   portPtr  GPIO port pointer.
 * @param   pin      GPIO pin number.
 */
static inline void SevenSeg_SetDig(GPIO_TypeDef *portPtr, uint16_t pin){
	assert(portPtr != NULL);
	HAL_GPIO_WritePin(portPtr, pin, (DIGIT_ACTIVE_LVL == DIGIT_ACTIVE_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
#endif
/** @} end of ControlInline */
#ifdef __cplusplus
}
#endif

#endif // __SEVENSEG_H__
