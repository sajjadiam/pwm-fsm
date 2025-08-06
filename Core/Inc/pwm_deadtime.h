#ifndef PWM_DEADTIME_H
#define PWM_DEADTIME_H

#include <stdint.h>

#define DTG_MAX_VALUE 0xFF

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Convert nanoseconds to DTG register encoding.
 *
 * @param[in] t_ns Desired dead-time in nanoseconds.
 * @return DTG register value (0..255).
 */
uint8_t DT_ConvertNsToDTG(double t_ns);

/**
 * @brief Convert DTG register encoding to nanoseconds.
 *
 * @param[in] dtg DTG register value.
 * @return Dead-time in nanoseconds.
 */
uint16_t DT_ConvertDTGToNs(uint8_t dtg);

/**
 * @brief Compute dead-time based on switching frequency.
 *
 * @param[in] freq_hz Switching frequency in Hz.
 * @return Dead-time in nanoseconds.
 */
double DT_FromFreq(uint16_t freq_hz);

/**
 * @brief Compute dead-time based on heat-sink temperature.
 *
 * @param[in] temp_c Heat-sink temperature in Celsius.
 * @return Dead-time in nanoseconds.
 */
uint16_t DT_FromTemp(float temp_c);

/**
 * @brief Compute appropriate dead-time (DTG) combining frequency and temperature constraints.
 *
 * @param[in] freq_hz Current switching frequency.
 * @param[in] temp_c Current heat-sink temperature.
 * @return DTG register value.
 */
uint8_t DT_ComputeTempFreq(uint16_t freq_hz, float temp_c);

/**
 * @brief Compute dead-time requirement based on instantaneous power.
 *
 * @param[in] voltage Measured voltage.
 * @param[in] current Measured current.
 * @param[in] freq_hz Current switching frequency.
 * @param[in] max_power Maximum allowable power.
 * @return DTG register value.
 */
uint8_t DT_ComputePower(float voltage, float current, uint16_t freq_hz, float max_power);

#ifdef __cplusplus
}
#endif

#endif // PWM_DEADTIME_H
