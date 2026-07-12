#ifndef SETTINGS_H
#define SETTINGS_H

/**
 * @file settings.h
 * @brief Project-wide configuration constants for Watering System.
 */

#include "settings_user.h"

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

// Soil moisture sensor pins
// Analog pin connected to sensor analog output (AO)
#define SOIL_MOISTURE_SENSOR_ANALOG_PIN 1

// Solar panel and battery pins
// Analog pin for solar panel voltage
#define SOLAR_VOLTAGE_PIN 3
// Analog pin for battery level
#define BATTERY_LEVEL_PIN 4

// Relay configuration
// GPIO pin controlling the relay module (pump/valve)
#define RELAY_PIN 2

// LED status indicator pins
#define LED_PIN_WIFI_CONNECTED 8
#define LED_PIN_PUMP_ACTIVE 7
#define LED_PIN_BATTERY_LOW 6


// =========================
// WiFi AUTO-RECONNECT CONFIGURATION
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

/**
 * @brief Delay (ms) at the end of each loop() iteration.
 */
#define LOOP_DELAY_MS               100

#endif // SETTINGS_H
