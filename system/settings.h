#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * @file settings.h
 * @brief Project-wide configuration constants for Watering System.
 */

#include "settings_user.h"

// =========================
// GPIO PIN ASSIGNMENTS
// =========================

/**
 * @brief GPIO pin number for the onboard LED (ESP32C3).
 */
#define LED_BUILTIN 8

/**
 * @brief GPIO pin connected to the push button.
 *        Button connects VCC to this pin. Internal pulldown enabled;
 *        add an external 10kΩ pulldown if unstable.
 */
#define BUTTON_PIN 0

/**
 * @brief GPIO pin controlling the relay module (pump/valve).
 */
#define RELAY_PIN 2


// =========================
// SOIL MOISTURE SENSOR CONFIGURATION
// =========================

/**
 * @brief Analog pin connected to the soil moisture sensor's analog output (AO).
 */
#define SOIL_MOISTURE_SENSOR_ANALOG_PIN 1

/**
 * @brief Soil moisture threshold (0-4095 ADC range).
 *        Values ABOVE this threshold = DRY, values BELOW = WET.
 */
#define SOIL_MOISTURE_THRESHOLD (4096 * 3 / 4)

/**
 * @brief Soil moisture floating threshold (0-4095 ADC range).
 *        Values BELOW this threshold = FLOATING (sensor not in soil / unreliable reading).
 */
#define SOIL_MOISTURE_FLOATING_THRESHOLD 100


// =========================
// SOLAR PANEL & BATTERY CONFIGURATION
// =========================

/**
 * @brief Analog pin for solar panel voltage.
 */
#define SOLAR_VOLTAGE_PIN 3

/**
 * @brief Analog pin for battery level.
 */
#define BATTERY_LEVEL_PIN 4

/**
 * @brief Resistor values for the solar panel voltage divider.
 *        Two 470k resistors are used.
 */
#define SOLAR_DIVIDER_R1 470000.0
#define SOLAR_DIVIDER_R2 470000.0

/**
 * @brief Resistor values for the battery voltage divider.
 *        Two 470k resistors are used.
 */
#define BATTERY_DIVIDER_R1 470000.0
#define BATTERY_DIVIDER_R2 470000.0


// =========================
// LED STATUS INDICATOR PINS
// =========================

#define LED_PIN_BATTERY_LOW    5
#define LED_PIN_AUTO_MODE      6
#define LED_PIN_PUMP_ACTIVE    7
#define LED_PIN_WIFI_CONNECTED 8

/**
 * @brief Battery voltage threshold for the low battery indicator.
 *        Values BELOW this threshold trigger the low battery LED.
 */
#define BATTERY_LOW_THRESHOLD 3.5f


// =========================
// WIFI AUTO-RECONNECT CONFIGURATION
// =========================

/**
 * @brief Base delay (ms) before the first WiFi reconnection attempt.
 *        Each subsequent retry doubles this value until WIFI_RETRY_MAX_DELAY_MS.
 */
#define WIFI_RETRY_BASE_DELAY_MS    10000  // 10 seconds

/**
 * @brief Maximum delay (ms) between WiFi reconnection attempts.
 */
#define WIFI_RETRY_MAX_DELAY_MS    600000  // 10 minutes


// =========================
// AUTOMATIC WATERING CONFIGURATION
// =========================

/**
 * @brief Duration (ms) the pump remains ON during automatic watering.
 */
#define AUTO_WATERING_DURATION_MS    10000  // 10 seconds

/**
 * @brief Interval (ms) between automatic watering cycles.
 */
#define AUTO_WATERING_INTERVAL_MS    (15 * 60 * 1000)  // 15 minutes

// =========================
// MAIN LOOP CONFIGURATION
// =========================

/**
 * @brief Delay (ms) at the end of each loop() iteration.
 */
#define LOOP_DELAY_MS    300

#endif // SETTINGS_H