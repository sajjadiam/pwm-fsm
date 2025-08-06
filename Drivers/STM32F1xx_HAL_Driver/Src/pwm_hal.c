#include "pwm_hal.h"
#include "stm32f1xx_hal.h"

#define CLK_FREQ     72000000     // Timer clock frequency (72 MHz)

extern TIM_HandleTypeDef htim1;   // Timer handle created in CubeMX
/**
 * @brief Initialize TIM1 with center-aligned mode and default PWM settings.
 *        Sets ARR based on START_FREQ, and sets 50% duty on CH1 and CH2.
 */
void HAL_PWM_Init(uint16_t freq) {
	uint32_t initArr = (CLK_FREQ / (2 * freq)) - 1;
	HAL_PWM_SetARR(initArr);
	HAL_PWM_SetCompare(TIM_CHANNEL_1, (initArr + 1) / 2);
	HAL_PWM_SetCompare(TIM_CHANNEL_2, (initArr + 1) / 2);
}
/**
 * @brief Set the Auto Reload Register (ARR) for TIM1.
 * @param arr New period value.
 */
void HAL_PWM_SetARR(uint32_t arr) {
	__HAL_TIM_SET_AUTORELOAD(&htim1, arr);
}
/**
 * @brief Set the compare value for the given TIM1 channel (duty cycle).
 * @param channel TIM_CHANNEL_1 or TIM_CHANNEL_2
 * @param compare Compare value for duty cycle
 */
void HAL_PWM_SetCompare(uint32_t channel, uint32_t compare) {
	__HAL_TIM_SET_COMPARE(&htim1, channel, compare);
}
/**
 * @brief Set dead time for complementary outputs (PWMN).
 * @param deadTime Value between 0 and 255 (DTG bits in BDTR register)
 */
void HAL_PWM_SetDeadTime(uint8_t deadTime) {
	MODIFY_REG(htim1.Instance->BDTR, TIM_BDTR_DTG, deadTime);
}
/**
 * @brief Start PWM output on both CH1/CH2 and their complementary (CH1N/CH2N).
 */
void HAL_PWM_Start(void) {
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}
/**
 * @brief Stop PWM output on both CH1/CH2 and their complementary channels.
 */
void HAL_PWM_Stop(void) {
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
}
/**
 * @brief Get the current Auto Reload Register value.
 * @return ARR value
 */
uint32_t HAL_PWM_GetARR(void){
    return __HAL_TIM_GET_AUTORELOAD(&htim1);
}
void HAL_PWM_Reset_Counter(void){
	__HAL_TIM_SET_COUNTER(&htim1, 0);
}
void HAL_PWM_TIMER_Disable(void){
	 __HAL_TIM_DISABLE(&htim1);
}
void HAL_PWM_TIMER_Enable(void){
	__HAL_TIM_ENABLE(&htim1);
}
/*End of pwm_hal.c*/