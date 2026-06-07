#include <Arduino.h>
#include "settings.h"
#include "wifi_handler.h"

/**
 * @file system.ino
 * @brief Main entry point for ESP32C3 Watering System.
 */

// Soil moisture sensor pins are defined in settings.h as
// SOIL_MOISTURE_SENSOR_ANALOG_PIN and SOIL_MOISTURE_SENSOR_DIGITAL_PIN

/**
 * @brief Arduino setup function. Initializes Serial, LED, WiFi and sensor pins.
 */
void setup() {
    Serial.begin(115200);
    Serial.println("Start");

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(SOIL_MOISTURE_SENSOR_DIGITAL_PIN, INPUT);

    connectToWiFi(WIFI_SSID, WIFI_PASS);
}

/**
 * @brief Arduino main loop.
 */
void loop() {
    // Read analog value (0-4095 on ESP32-C3)
    int soilAnalog = analogRead(SOIL_MOISTURE_SENSOR_ANALOG_PIN);

    // Read digital value from LM393 comparator
    int soilDigital = digitalRead(SOIL_MOISTURE_SENSOR_DIGITAL_PIN);

    Serial.print("Soil Analog: ");
    Serial.print(soilAnalog);

    Serial.print(" | Soil Digital: ");
    Serial.print(soilDigital);

    Serial.print(" | Status: ");
    Serial.println(soilDigital ? "DRY" : "WET");

    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(800);
}