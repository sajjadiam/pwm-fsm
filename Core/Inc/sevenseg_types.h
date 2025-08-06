/* ===========================================================================

@file    sevenseg_types.h

@brief   Data types and enums for 7-segment display library

@author  Your Name

@date    2025-07-28

=========================================================================== */
#ifndef SEVENSEG_TYPES_H
#define SEVENSEG_TYPES_H

#include <stdint.h>

/**

@enum SegmentActiveLevel_t

@brief Defines possible active levels for segment pins.
*/

#define SEGMENT_ACTIVE_LOW   0  	/* Segment on when pin is low */
#define SEGMENT_ACTIVE_HIGH  1   	/* Segment on when pin is high */


/**

@enum DigitActiveLevel_t

@brief Defines possible active levels for digit enable pins.
*/

#define DIGIT_ACTIVE_LOW   0   		/* Digit enable on low */
#define	DIGIT_ACTIVE_HIGH  1     	/* Digit enable on high */


/**

@enum SegmentBitOrder_t

@brief Defines bit ordering for segment masks.
*/

#define	LSB_FIRST  0  	/* Plain mask: MSB=SEG_A, LSB=SEG_DP */
#define MSB_FIRST  1   	/* Struct mask: LSB=SEG_A, MSB=SEG_DP */

#endif // SEVENSEG_TYPES_H