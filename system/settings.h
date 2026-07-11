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

// Soil moisture sensor pins
// Analog pin connected to sensor analog output (AO)
#define SOIL_MOISTURE_SENSOR_ANALOG_PIN 0
// Digital pin connected to sensor digital output (DO)
#define SOIL_MOISTURE_SENSOR_DIGITAL_PIN 1

// Solar panel and battery pins
// Analog pin for solar panel voltage
#define SOLAR_VOLTAGE_PIN 3
// Analog pin for battery level
#define BATTERY_LEVEL_PIN 4

// Relay configuration
// GPIO pin controlling the relay module (pump/valve)
#define RELAY_PIN 2
// Set to 1 if the relay module is active LOW (common modules), 0 if active HIGH
#define RELAY_ACTIVE_LOW 1

// LED status indicator pins
#define LED_PIN_WIFI_CONNECTED 5
#define LED_PIN_PUMP_ACTIVE 6
#define LED_PIN_BATTERY_LOW 7

#if RELAY_ACTIVE_LOW
#define RELAY_ON LOW
#define RELAY_OFF HIGH
#else
#define RELAY_ON HIGH
#define RELAY_OFF LOW
#endif

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
 * @brief If WiFi fails to connect at startup, total time (ms) to keep retrying
 *        in setup() before falling through. After this, the loop() handles retries.
 */
#define WIFI_STARTUP_RETRY_PERIOD   30000  // 30 seconds

#endif // SETTINGS_H
