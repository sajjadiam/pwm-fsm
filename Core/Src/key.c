#include "key.h"
/**
 * @brief Initialize key configurations.
 *
 * This function is optional and currently empty because
 * GPIO initialization is handled by CubeMX or elsewhere.
 *
 * @param key Pointer to an array of KeyPinConfig structs.
 * @param len Number of keys in the array.
 */
void Key_Init(KeyPinConfig* key, uint16_t len) {
	// You can optionally add dynamic GPIO config here
}
/**
 * @brief Reads and updates the current key states.
 *
 * Uses 2-bit shift logic to track key states over time.
 * Should be called regularly (e.g., every 30ms) to properly detect key presses/releases.
 *
 * @param key Pointer to an array of KeyPinConfig structs.
 * @param len Number of keys in the array.
 */
void Key_Read(KeyPinConfig* key, uint16_t len) {
	while (len-- > 0) {
		// Shift left by 1, add current pin state bit, and mask to keep only 2 bits
		key->state = ((key->state << 1) | ((key->GPIOPort->IDR & key->GPIOPin) ? 1 : 0)) & 0x03;
		key++;
	}
}
/*End of key.c*/