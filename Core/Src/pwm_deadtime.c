/**
 * @file pwm_deadtime.c
 * @brief Dead-Time Computation Module Implementation
 *
 * This module contains functions to convert and compute Dead-Time values for PWM.
 * It includes conversions between nanoseconds and DTG register values,
 * as well as calculations based on frequency, temperature, and power.
 */
#include "pwm_deadtime.h"
#include "pwm_config.h"   /**< Contains BASE_DEAD_TIME, DEAD_TIME__PER_HZ, CLK_FREQ, START_FREQ, etc. */
#include <math.h>
/**
 * @brief Converts nanoseconds to DTG register value.
 *
 * According to STM32 hardware specifications, dead-time is encoded in multiple regions.
 * This function converts the requested dead-time in nanoseconds to the 8-bit DTG register value.
 *
 * @param t_ns Dead-time in nanoseconds.
 * @return 8-bit DTG value to be programmed into the register.
 */
uint8_t DT_ConvertNsToDTG(double t_ns) {
	double t_dts = 1e9 / (double)PWM_CLK_FREQ;  /**< Nanoseconds per timer tick */
	uint16_t ticks;
	// Region 0: linear 0..127
	ticks = (uint16_t)ceil(t_ns / t_dts);
	if (ticks <= 127){
		return (uint8_t)ticks;
	}
	// Region 1: (64 + x)*2*t_dts
	ticks = (uint16_t)ceil(t_ns / (2 * t_dts));
	if (ticks <= 127){
		return (uint8_t)(64 + (ticks - 64));
	}
	// Region 2: (32 + x)*8*t_dts
	ticks = (uint16_t)ceil(t_ns / (8 * t_dts));
	if (ticks <= 63){
		return (uint8_t)(192 + (ticks - 32));
	}
	// Region 3: (32 + x)*16*t_dts
	ticks = (uint16_t)ceil(t_ns / (16 * t_dts));
	if (ticks <= 63){
		return (uint8_t)(224 + (ticks - 32));
	}
	return 255; /**< Invalid value */
}
/**
 * @brief Converts DTG register value back to nanoseconds.
 *
 * Inverse of DT_ConvertNsToDTG. Returns the dead-time in nanoseconds for a given DTG value.
 *
 * @param dtg 8-bit DTG register value.
 * @return Dead-time in nanoseconds.
 */
uint16_t DT_ConvertDTGToNs(uint8_t dtg) {
	double t_dts = 1e9 / (double)PWM_CLK_FREQ;
	uint8_t mode = dtg >> 5;
	if(mode <= 3){
		return (uint16_t)ceil(dtg * t_dts);
	} 
	else if(mode <= 5){
		uint8_t x = dtg & 0x3F;
		return (uint16_t)ceil((64 + x) * 2 * t_dts);
	} 
	else if(mode == 6){
		uint8_t x = dtg & 0x1F;
		return (uint16_t)ceil((32 + x) * 8 * t_dts);
	} 
	else{ // mode == 7
		uint8_t x = dtg & 0x1F;
		return (uint16_t)ceil((32 + x) * 16 * t_dts);
	}
	return (uint8_t)DTG_MAX_VALUE;
}
/**
 * @brief Computes dead-time based on PWM frequency.
 *
 * Returns the base dead-time if frequency is less than or equal to MIN_FREQ.
 * Otherwise, dead-time increases linearly with frequency.
 *
 * @param freq_hz PWM frequency in Hertz.
 * @return Dead-time in nanoseconds.
 */
double DT_FromFreq(uint16_t freq_hz) {
	if(freq_hz <= PWM_MIN_FREQ){
		return PWM_BASE_DEAD_TIME;
	}
	double dt_ns = PWM_BASE_DEAD_TIME + (PWM_DEAD_TIME_PER_HZ * (double)(freq_hz - PWM_START_FREQ));
	return dt_ns;
}
/**
 * @brief Computes dead-time based on temperature in degrees Celsius.
 *
 * For temperatures <= 25°C, returns base dead-time.
 * Between 25°C and 125°C, dead-time increases linearly.
 * Above 125°C, dead-time increases with a factor of 1.5.
 *
 * @param temp_c Temperature in degrees Celsius.
 * @return Dead-time in nanoseconds.
 */
uint16_t DT_FromTemp(float temp_c) {
	const float kT = 1.0f;  /**< ns per °C */
	if (temp_c <= 25.0f){
		return PWM_BASE_DEAD_TIME;
	}
	if (temp_c >= 125.0f){
		return (uint16_t)ceil(PWM_BASE_DEAD_TIME + kT * 1.5f * (temp_c - 25.0f));
	}
	return (uint16_t)ceil(PWM_BASE_DEAD_TIME + kT * (temp_c - 25.0f));
}
/**
 * @brief Computes optimal dead-time based on temperature and frequency.
 *
 * Calculates dead-time separately for temperature and frequency,
 * and returns the larger value as the final dead-time.
 *
 * @param freq_hz PWM frequency in Hertz.
 * @param temp_c Temperature in degrees Celsius.
 * @return Encoded 8-bit DTG value for dead-time register.
 */
uint8_t DT_ComputeTempFreq(uint16_t freq_hz, float temp_c) {
	uint8_t dt_freq = DT_ConvertNsToDTG(DT_FromFreq(freq_hz));
	uint8_t dt_temp = DT_ConvertNsToDTG(DT_FromTemp(temp_c));
	return dt_freq > dt_temp ? dt_freq : dt_temp;
}
/**
 * @brief Computes dead-time based on power consumption and max allowed power.
 *
 * If power is less than or equal to max_power, returns base dead-time.
 * Otherwise, computes additional dead-time proportional to excess power.
 * If the computed dead-time exceeds half the PWM period, returns 0.
 *
 * @param voltage Voltage in volts.
 * @param current Current in amperes.
 * @param freq_hz PWM frequency in Hertz.
 * @param max_power Maximum allowed power in watts.
 * @return Encoded 8-bit DTG value for dead-time register.
 */
uint8_t DT_ComputePower(float voltage, float current, uint16_t freq_hz, float max_power) {
	double power = voltage * current;
	if(freq_hz == 0 || power == 0){
		return DT_ConvertNsToDTG(PWM_BASE_DEAD_TIME);
	}
	if(power <= max_power) {
		return DT_ConvertNsToDTG(PWM_BASE_DEAD_TIME);
	}
	double period = 1.0 / freq_hz;
	double deltaP = power - max_power;
	double dt_ns = (deltaP * period) / (2.0 * power) * 1e9;
	if (dt_ns >= (period / 2.0) * 1e9) {
			return 0;
	}
	return DT_ConvertNsToDTG((uint16_t)ceil(dt_ns));
}
/*End of file pwm_deadtime.c*/