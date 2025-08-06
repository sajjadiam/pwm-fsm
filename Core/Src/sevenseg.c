/* ===========================================================================
 * @file    sevenseg.c
 * @brief   Implementation of 7-segment display library.
 *          Provides initialization, update, buffer handling, and utility.
 *
 * @author  Your Name
 * @date    2025-07-28
 * ===========================================================================
 */
#include <string.h>
#include <assert.h>
#include "sevenseg.h"
/**
 * @brief  Initialize the 7-segment display handle.
 * @param  hseg  Pointer to a pre-configured SevenSeg_HandleTypeDef.
 * @return None
 */
void SevenSeg_Init(SevenSeg_HandleTypeDef *hseg){
	assert(hseg != NULL);
	/* Segment pins A..DP */
	hseg->segmentPins[0] = SEGMENT_A_GPIO_PIN;
	hseg->segmentPins[1] = SEGMENT_B_GPIO_PIN;
	hseg->segmentPins[2] = SEGMENT_C_GPIO_PIN;
	hseg->segmentPins[3] = SEGMENT_D_GPIO_PIN;
	hseg->segmentPins[4] = SEGMENT_E_GPIO_PIN;
	hseg->segmentPins[5] = SEGMENT_F_GPIO_PIN;
	hseg->segmentPins[6] = SEGMENT_G_GPIO_PIN;
	hseg->segmentPins[7] = SEGMENT_DP_GPIO_PIN;
#if (USE_COMMON_SEGMENT_PORT)
	hseg->segmentPort = SEGMENTS_GPIO_PORT;
#else
	/* Individual segment ports if not common */
	hseg->segmentPorts[0] = SEGMENT_A_GPIO_PORT;
	hseg->segmentPorts[1] = SEGMENT_B_GPIO_PORT;
	hseg->segmentPorts[2] = SEGMENT_C_GPIO_PORT;
	hseg->segmentPorts[3] = SEGMENT_D_GPIO_PORT;
	hseg->segmentPorts[4] = SEGMENT_E_GPIO_PORT;
	hseg->segmentPorts[5] = SEGMENT_F_GPIO_PORT;
	hseg->segmentPorts[6] = SEGMENT_G_GPIO_PORT;
	hseg->segmentPorts[7] = SEGMENT_DP_GPIO_PORT;
#endif
	/* Digit pins D1..D4 */
	hseg->digitPins[0] = DIGIT_1_GPIO_PIN;
	hseg->digitPins[1] = DIGIT_2_GPIO_PIN;
	hseg->digitPins[2] = DIGIT_3_GPIO_PIN;
	hseg->digitPins[3] = DIGIT_4_GPIO_PIN;
#if (USE_COMMON_DIGIT_PORT)
	hseg->digitPort = DIGITS_GPIO_PORT;
#else
	/* Individual digit ports if not common */
	hseg->digitPorts[0] = DIGIT_1_GPIO_PORT;
	hseg->digitPorts[1] = DIGIT_2_GPIO_PORT;
	hseg->digitPorts[2] = DIGIT_3_GPIO_PORT;
	hseg->digitPorts[3] = DIGIT_4_GPIO_PORT;
#endif
	hseg->currentDigit = 0;
	memset(hseg->buffer, 0, DIGIT_NUM);
}
/**
 * @brief  Refresh/update the displayed digit (multiplexing).
 *         Should be called periodically (e.g., from timer ISR).
 * @param  hseg  Pointer to the display handle.
 * @return None
 */
void SevenSeg_Update(SevenSeg_HandleTypeDef *hseg){
	assert(hseg != NULL);
	/* Turn off all digits */
#if (USE_COMMON_DIGIT_PORT == 0)
	for(int i = 0; i < DIGIT_NUM; ++i){
		SevenSeg_ClrDig(hseg->digitPorts[i], hseg->digitPins[i]);
	}
#else
	SevenSeg_ClrDigits(DIGITS_GPIO_PORT, DIGITS_MASK_ALL);
#endif
	/* Set segments for current digit */
#if (USE_COMMON_SEGMENT_PORT == 0)
	uint8_t segs = hseg->buffer[hseg->currentDigit];
	for(int i = 0; i < SEGMENT_NUM; ++i){
		if(segs & (1U << i)){
			SevenSeg_SetSeg(hseg->segmentPorts[i], hseg->segmentPins[i]);
		} 
		else{
			SevenSeg_ClrSeg(hseg->segmentPorts[i], hseg->segmentPins[i]);
		}
	}
#else
	SevenSeg_ClrSegments(SEGMENTS_GPIO_PORT, SEGMENT_MASK_ALL);
	SevenSeg_SetSegments(SEGMENTS_GPIO_PORT, hseg->buffer[hseg->currentDigit],SEGMENT_MASK_ALL,SEGMENT_PIN_SHIFT);
#endif
	/* Turn on current digit */
#if (USE_COMMON_DIGIT_PORT == 0)
	SevenSeg_SetDig(hseg->digitPorts[hseg->currentDigit],hseg->digitPins[hseg->currentDigit]);
#else
	SevenSeg_SetDigits(DIGITS_GPIO_PORT, hseg->digitPins[hseg->currentDigit], DIGITS_MASK_ALL);
#endif
	/* Advance to next digit */
	hseg->currentDigit = (hseg->currentDigit + 1) % DIGIT_NUM;
}
/**
 * @brief  Load text string into the display buffer.
 * @param  hseg  Pointer to the display handle.
 * @param  str   Null-terminated input string (max length = DIGIT_NUM + dots).
 * @return None
 */
void SevenSeg_BufferUpdate(SevenSeg_HandleTypeDef *hseg,char *str){
	assert(hseg != NULL);
	assert(str  != NULL);
	size_t len   = strlen(str);
	size_t count = 0;
	/* Map non-dot characters to buffer */
	for (size_t i = 0; i < len && count < DIGIT_NUM; i++){
		if (str[i] != '.'){
			hseg->buffer[count++] = segmentMapASCII[(uint8_t)str[i]];
		}
	}
	/* Handle decimal point */
	int8_t dotIndex = SevenSeg_GetDotIndex(str);
	if(dotIndex >= 0 && dotIndex < DIGIT_NUM){
		hseg->buffer[dotIndex] |= segmentMapASCII[(uint8_t)'.'];
	}
}
/**
 * @brief  Find decimal point index in string (before dot).
 * @param  str  Null-terminated input string.
 * @return Index of digit preceding '.', or -1 if none.
 */
int8_t SevenSeg_GetDotIndex(char *str){
	assert(str != NULL);
	for(int8_t i = 0; str[i] != '\0'; ++i){
		if(str[i] == '.'){
			return (int8_t)(i > 0 ? i - 1 : -1);
		}
	}
	return -1;
}
/* End of file sevenseg.c */