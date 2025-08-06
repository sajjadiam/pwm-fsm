#ifndef __PWM_CONFIG_H__
#define __PWM_CONFIG_H__
/**
 * @file pwm_config.h
 * @brief PWM module configuration macros and constants.
 */ 
/// Utility macros for common operations
#define PWM_MAX(a,b)        (((a) > (b)) ? (a) : (b))       /**< Returns maximum of a and b */
#define PWM_MIN(a,b)        (((a) < (b)) ? (a) : (b))       /**< Returns minimum of a and b */
#define PWM_CLAMP(val, min, max)  ((val) < (min) ? (min) : ((val) > (max) ? (max) : (val))) /**< Clamp val between min and max */
/**
 * @def PWM_TRUE
 * @brief Boolean true value.
 */
#define PWM_TRUE            1
/**
 * @def PWM_FALSE
 * @brief Boolean false value.
 */
#define PWM_FALSE           0
/**
 * @def PWM_START_FREQ
 * @brief Starting PWM frequency in Hertz.
 */
#define PWM_START_FREQ      10000UL
/**
 * @def PWM_END_FREQ
 * @brief Ending PWM frequency in Hertz.
 */
#define PWM_END_FREQ        20000UL
/**
 * @def PWM_CLK_FREQ
 * @brief Timer clock frequency in Hertz.
 */
#define PWM_CLK_FREQ        72000000UL
/**
 * @def PWM_START_ARR
 * @brief Auto-reload register value at starting frequency.
 */
#define PWM_START_ARR       ((PWM_CLK_FREQ / (2 * PWM_START_FREQ)) - 1U)
/**
 * @def PWM_END_ARR
 * @brief Auto-reload register value at ending frequency.
 */
#define PWM_END_ARR         ((PWM_CLK_FREQ / (2 * PWM_END_FREQ)) - 1U)
/**
 * @def PWM_START_DEAD_TIME
 * @brief Dead time at the start (maximum value).
 */
#define PWM_START_DEAD_TIME 255U
/**
 * @def PWM_END_DEAD_TIME
 * @brief Dead time at the end (minimum value).
 */
#define PWM_END_DEAD_TIME   75U
/**
 * @def PWM_SOFT_START_TIME
 * @brief Number of steps for soft start transition.
 */
#define PWM_SOFT_START_TIME (PWM_START_DEAD_TIME - PWM_END_DEAD_TIME)
/**
 * @def PWM_SOFT_START_STEP
 * @brief Auto-reload register decrement per soft start step.
 */
#define PWM_SOFT_START_STEP (PWM_START_ARR - PWM_END_ARR) / PWM_SOFT_START_TIME
/**
 * @def PWM_MIN_FREQ
 * @brief Minimum allowable PWM frequency in Hertz.
 */
#define PWM_MIN_FREQ        20000UL
/**
 * @def PWM_MAX_FREQ
 * @brief Maximum allowable PWM frequency in Hertz.
 */
#define PWM_MAX_FREQ        50000UL
/**
 * @def PWM_MAX_ARR
 * @brief Auto-reload register value at maximum frequency.
 */
#define PWM_MAX_ARR         ((PWM_CLK_FREQ / (2 * PWM_MAX_FREQ)) - 1U)
/**
 * @def PWM_MIN_ARR
 * @brief Auto-reload register value at minimum frequency.
 */
#define PWM_MIN_ARR         ((PWM_CLK_FREQ / (2 * PWM_MIN_FREQ)) - 1U)
/**
 * @def PWM_SOFT_CHANGE_STEP
 * @brief Step size for frequency or ARR change during soft transitions.
 */
#define PWM_SOFT_CHANGE_STEP 1U
/**
 * @def PWM_BASE_DEAD_TIME
 * @brief Base dead time in nanoseconds.
 */
#define PWM_BASE_DEAD_TIME  200U
/**
 * @def PWM_DEAD_TIME_PER_HZ
 * @brief Dead time increment per Hertz (nanoseconds per Hz).
 */
#define PWM_DEAD_TIME_PER_HZ 0.0005f
/**
 * @def PWM_MAX_POWER
 * @brief Maximum allowable power (unit depends on system, e.g., watts).
 */
#define PWM_MAX_POWER       2000U
/**
 * @def PWM_DT_PWR_RATE
 * @brief Power-related dead time rate factor (scaling factor).
 */
#define PWM_DT_PWR_RATE     0.2f
#endif // __PWM_CONFIG_H__
