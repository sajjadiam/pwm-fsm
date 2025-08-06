/* ===========================================================================

@file    sevenseg_config.h

@brief   User configuration macros and validation for 7-segment library

=========================================================================== */
#ifndef SEVENSEG_CONFIG_H
#define SEVENSEG_CONFIG_H

#include "sevenseg_types.h"

/** @name Active Level Configuration */
///@{
#define SEGMENT_ACTIVE_LVL   SEGMENT_ACTIVE_HIGH  /* Segment active level */
#define DIGIT_ACTIVE_LVL     DIGIT_ACTIVE_HIGH    /* Digit active level */
///@}

/* Validate active-level configuration */
#if (SEGMENT_ACTIVE_LVL != SEGMENT_ACTIVE_HIGH && SEGMENT_ACTIVE_LVL != SEGMENT_ACTIVE_LOW)
#error "SEGMENT_ACTIVE_LVL must be SEGMENT_ACTIVE_LOW or SEGMENT_ACTIVE_HIGH"
#endif
#if (DIGIT_ACTIVE_LVL != DIGIT_ACTIVE_HIGH && DIGIT_ACTIVE_LVL != DIGIT_ACTIVE_LOW)
#error "DIGIT_ACTIVE_LVL must be DIGIT_ACTIVE_LOW or DIGIT_ACTIVE_HIGH"
#endif

/** @name Display Geometry */
///@{
#define DIGIT_NUM   4         /* Number of digits (1..8) */
#define SEGMENT_NUM 8         /* Number of segments per digit (7 + DP) */
///@}

/* Validate geometry */
#if (DIGIT_NUM < 1 || DIGIT_NUM > 8)
#error "DIGIT_NUM must be between 1 and 8"
#endif
#if (SEGMENT_NUM != 7 && SEGMENT_NUM != 8)
#error "SEGMENT_NUM must be 7 or 8"
#endif

/** @brief Bit ordering for segment masks */
#define SEGMENT_STRUCTURE	MSB_FIRST
#if (SEGMENT_STRUCTURE != LSB_FIRST && SEGMENT_STRUCTURE != MSB_FIRST)
#error "SEGMENT_STRUCTURE must be LSB_FIRST or MSB_FIRST"
#endif

#endif // SEVENSEG_CONFIG_H