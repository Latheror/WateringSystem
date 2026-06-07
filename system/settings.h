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

#endif // SETTINGS_H
