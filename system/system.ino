#include <Arduino.h>
#include <WebServer.h>
#include "settings.h"
#include "wifi_handler.h"
#include "pump_handler.h"
#include "webapp.h"
WebServer server(80);

/**
 * @file system.ino
 * @brief Main entry point for ESP32C3 Watering System.
 */


// Soil moisture sensor pins are defined in settings.h as
// SOIL_MOISTURE_SENSOR_ANALOG_PIN and SOIL_MOISTURE_SENSOR_DIGITAL_PIN

// Global variables for web app
// Non-blocking variables for watering

/**
 * @brief Arduino setup function. Initializes Serial, LED, WiFi and sensor pins.
 */
void setup() {
    Serial.begin(115200);
    Serial.println("Start");

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(SOIL_MOISTURE_SENSOR_DIGITAL_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(SOLAR_VOLTAGE_PIN, INPUT);
    pinMode(BATTERY_LEVEL_PIN, INPUT);

    // Ensure relay starts off
    digitalWrite(RELAY_PIN, RELAY_OFF);

    connectToWiFi(WIFI_SSID, WIFI_PASS);


    // Initialize web app routes
    initWebApp();
    
    Serial.println("Web server started on port 80");}


/**
 * @brief Arduino main loop.
 */
void loop() {
    server.handleClient();

    // Read analog value (0-4095 on ESP32-C3)
    int soilAnalog = analogRead(SOIL_MOISTURE_SENSOR_ANALOG_PIN);

    // Read digital value from LM393 comparator
    int soilDigital = digitalRead(SOIL_MOISTURE_SENSOR_DIGITAL_PIN);

    // Read solar and battery voltages
    int solarRaw = analogRead(SOLAR_VOLTAGE_PIN);
    int batteryRaw = analogRead(BATTERY_LEVEL_PIN);

    Serial.print("Soil Analog: ");
    Serial.print(soilAnalog);

    Serial.print(" | Soil Digital: ");
    Serial.print(soilDigital);

    Serial.print(" | Status: ");
    Serial.println(soilDigital ? "DRY" : "WET");

    // Control relay: turn ON when soil is DRY, OFF when WET (only if not manually watering)
    if (!isWatering) {
        if (soilDigital) {
            setPumpState(true);
            Serial.println("Relay: ON");
        } else {
            setPumpState(false);
            Serial.println("Relay: OFF");
        }
    }

    // Update global variables for web app
    solarVoltage = (solarRaw * 3.3) / 4095.0;
    batteryVoltage = (batteryRaw * 3.3) / 4095.0;
    pumpActive = (digitalRead(RELAY_PIN) == RELAY_ON);

    Serial.print("Solar: ");
    Serial.print(solarVoltage);
    Serial.print("V | Battery: ");
    Serial.print(batteryVoltage);
    Serial.println("V");

    // Non-blocking watering timer
    if (isWatering && (millis() - wateringStartTime >= 10000)) {
        setPumpState(false);
        isWatering = false;
        Serial.println("Watering complete");
    }

    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(800);

}