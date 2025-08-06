#ifndef __PWM_HAL_H__
#define __PWM_HAL_H__
/**
 * @file pwm_hal.h
 * @brief High-level interface for configuring center-aligned PWM using TIM1
 * @author -
 */
//includes---extends
#include <stdint.h>
/**
 * @brief Initialize TIM1 and GPIO for center-aligned PWM.
 *        It also sets the ARR and duty to default startup values.
 */
void HAL_PWM_Init(uint16_t freq);
/**
 * @brief Set the PWM period (ARR - Auto Reload Register).
 * @param arr Value to set for ARR (defines PWM period).
 */
void HAL_PWM_SetARR(uint32_t arr);
/**
 * @brief Set PWM duty cycle (compare value) for a given channel.
 * @param channel TIM channel (e.g. TIM_CHANNEL_1)
 * @param compare Compare value to set (duty cycle)
 */
void HAL_PWM_SetCompare(uint32_t channel, uint32_t compare);
/**
 * @brief Set the dead time for complementary PWM channels.
 * @param deadTime Dead time in clock ticks (0-255)
 */
void HAL_PWM_SetDeadTime(uint8_t deadTime);
/**
 * @brief Start the PWM output on both main and complementary channels.
 */
void HAL_PWM_Start(void);
/**
 * @brief Stop the PWM output on both main and complementary channels.
 */
void HAL_PWM_Stop(void);
/**
 * @brief Get the current value of the Auto Reload Register (ARR).
 * @return The ARR register value (PWM period).
 */
uint32_t HAL_PWM_GetARR(void);
//---------------------------------extends
void HAL_PWM_Reset_Counter(void);
void HAL_PWM_TIMER_Disable(void);
void HAL_PWM_TIMER_Enable(void);
#endif // __PWM_HAL_H__