/* ===========================================================================

@file    sevenseg_pins.h

@brief   GPIO pin and port assignments for 7-segment library

=========================================================================== */
#ifndef SEVENSEG_PINS_H
#define SEVENSEG_PINS_H

#include "stm32f1xx_hal.h" // or appropriate HAL header

/** @brief Use common GPIO port for all segment pins (1=yes, 0=no) */
#define USE_COMMON_SEGMENT_PORT 1

/* Segment GPIO pin definitions */
#define SEGMENT_A_GPIO_PIN     GPIO_PIN_4
#define SEGMENT_B_GPIO_PIN     GPIO_PIN_5
#define SEGMENT_C_GPIO_PIN     GPIO_PIN_6
#define SEGMENT_D_GPIO_PIN     GPIO_PIN_7
#define SEGMENT_E_GPIO_PIN     GPIO_PIN_8
#define SEGMENT_F_GPIO_PIN     GPIO_PIN_9
#define SEGMENT_G_GPIO_PIN     GPIO_PIN_10
#define SEGMENT_DP_GPIO_PIN    GPIO_PIN_11

#if (USE_COMMON_SEGMENT_PORT)
#define SEGMENTS_GPIO_PORT  GPIOB
#define SEGMENT_MASK_ALL    (SEGMENT_A_GPIO_PIN | SEGMENT_B_GPIO_PIN | SEGMENT_C_GPIO_PIN | SEGMENT_D_GPIO_PIN | SEGMENT_E_GPIO_PIN | SEGMENT_F_GPIO_PIN | SEGMENT_G_GPIO_PIN | SEGMENT_DP_GPIO_PIN)
#define SEGMENT_PIN_SHIFT   (__builtin_ctz(SEGMENT_A_GPIO_PIN))
#else
/* Individual segment port definitions */
#define SEGMENT_A_GPIO_PORT   GPIOB
#define SEGMENT_B_GPIO_PORT   GPIOB
#define SEGMENT_C_GPIO_PORT   GPIOB
#define SEGMENT_D_GPIO_PORT   GPIOB
#define SEGMENT_E_GPIO_PORT   GPIOB
#define SEGMENT_F_GPIO_PORT   GPIOB
#define SEGMENT_G_GPIO_PORT   GPIOB
#define SEGMENT_DP_GPIO_PORT  GPIOB
#endif

/** @brief Use common GPIO port for all digit pins (1=yes, 0=no) */
#define USE_COMMON_DIGIT_PORT 1

/* Digit GPIO pin definitions */
#define DIGIT_1_GPIO_PIN       GPIO_PIN_7
#define DIGIT_2_GPIO_PIN       GPIO_PIN_6
#define DIGIT_3_GPIO_PIN       GPIO_PIN_5
#define DIGIT_4_GPIO_PIN       GPIO_PIN_4

#if (USE_COMMON_DIGIT_PORT)
#define DIGITS_GPIO_PORT    GPIOA
#define DIGITS_MASK_ALL     (DIGIT_1_GPIO_PIN | DIGIT_2_GPIO_PIN | DIGIT_3_GPIO_PIN | DIGIT_4_GPIO_PIN)
#define DIGIT_PIN_SHIFT   	(__builtin_ctz(DIGIT_4_GPIO_PIN))
#else
/* Individual digit port definitions */
#define DIGIT_1_GPIO_PORT   GPIOA
#define DIGIT_2_GPIO_PORT   GPIOA
#define DIGIT_3_GPIO_PORT   GPIOA
#define DIGIT_4_GPIO_PORT   GPIOA
#endif

#endif // SEVENSEG_PINS_H