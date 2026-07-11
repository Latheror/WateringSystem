#ifndef BUTTON_H
#define BUTTON_H

/**
 * @file button.h
 * @brief Push button driver with debounce support.
 *
 * Wiring: button connects VCC to BUTTON_PIN.
 *         Internal pulldown is enabled so the pin reads LOW when released
 *         and HIGH when pressed.
 */

#include <cstdint>

/**
 * @brief Debounce delay in milliseconds.
 *        Any state change shorter than this is ignored as noise.
 */
constexpr unsigned long BUTTON_DEBOUNCE_MS = 50;

/**
 * @brief Initialise the button pin.
 *        Must be called once in setup().
 */
void initButton();

/**
 * @brief Return true while the button is physically pressed (debounced).
 *        Call frequently (every loop iteration) to keep the internal state fresh.
 */
bool isButtonPressed();

/**
 * @brief Return true only once per press-release cycle.
 *        Useful for one-shot actions (toggle, trigger, etc.).
 *        Call frequently (every loop iteration).
 */
bool wasButtonPressed();

#endif // BUTTON_H
