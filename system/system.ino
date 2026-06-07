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
    pinMode(RELAY_PIN, OUTPUT);

    // Define relay on/off levels based on module wiring
#if RELAY_ACTIVE_LOW
#define RELAY_ON LOW
#define RELAY_OFF HIGH
#else
#define RELAY_ON HIGH
#define RELAY_OFF LOW
#endif

    // Ensure relay starts off
    digitalWrite(RELAY_PIN, RELAY_OFF);

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

    // Control relay: turn ON when soil is DRY, OFF when WET
    if (soilDigital) {
        digitalWrite(RELAY_PIN, RELAY_ON);
        Serial.println("Relay: ON");
    } else {
        digitalWrite(RELAY_PIN, RELAY_OFF);
        Serial.println("Relay: OFF");
    }

    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(800);
}