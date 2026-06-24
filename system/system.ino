#include <Arduino.h>
#include "settings.h"
#include "wifi_handler.h"

/**
 * @file system.ino
 * @brief Main entry point for ESP32C3 Watering System.
 */

// Soil moisture sensor pins are defined in settings.h as
// SOIL_MOISTURE_SENSOR_ANALOG_PIN and SOIL_MOISTURE_SENSOR_DIGITAL_PIN

// Global variables for web app
float solarVoltage = 0.0;
float batteryVoltage = 0.0;
bool pumpActive = false;

// Non-blocking variables for watering
unsigned long wateringStartTime = 0;
bool isWatering = false;

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

    // Web server routes
    server.on("/", handleRoot);
    server.on("/water", handleManualWatering);
    server.begin();
    Serial.println("Web server started on port 80");}

/**
 * @brief Handle root page request.
 */
void handleRoot() {
    String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    html += "<title>Watering System Dashboard</title>";
    html += "<style>";
    html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #eef2f7; margin: 0; padding: 20px; color: #333; }";
    html += ".container { max-width: 600px; margin: 0 auto; }";
    html += "h1 { text-align: center; color: #2c3e50; margin-bottom: 30px; }";
    html += ".grid { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; }";
    html += ".card { background: white; padding: 20px; border-radius: 15px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); text-align: center; }";
    html += ".card h2 { margin: 0 0 10px 0; font-size: 0.9em; color: #7f8c8d; text-transform: uppercase; }";
    html += ".card p { margin: 0; font-size: 1.5em; font-weight: bold; }";
    html += ".status-wet { color: #27ae60; }";
    html += ".status-dry { color: #e67e22; }";
    html += ".pump-on { color: #e74c3c; animation: blink 1s infinite; }";
    html += ".pump-off { color: #95a5a6; }";
    html += "@keyframes blink { 0% { opacity: 1; } 50% { opacity: 0.5; } 100% { opacity: 1; } }";
    html += ".btn-container { margin-top: 30px; text-align: center; }";
    html += "button { padding: 15px 30px; font-size: 1.1em; background-color: #3498db; color: white; border: none; border-radius: 50px; cursor: pointer; transition: background 0.3s; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }";
    html += "button:active { transform: scale(0.98); }";
    html += "button:hover { background-color: #2980b9; }";
    html += "</style></head><body>";
    html += "<div class='container'>";
    html += "<h1>🌿 Watering System</h1>";
    html += "<div class='grid'>";
    
    // Soil Status Card
    html += "<div class='card'><h2>Soil Status</h2>";
    html += "<p class='status-" + String(digitalRead(SOIL_MOISTURE_SENSOR_DIGITAL_PIN) ? "dry" : "wet") + "'>" + String(digitalRead(SOIL_MOISTURE_SENSOR_DIGITAL_PIN) ? "DRY" : "WET") + "</p></div>";
    
    // Solar Voltage Card
    html += "<div class='card'><h2>Solar Voltage</h2>";
    html += "<p>" + String(solarVoltage, 2) + " V</p></div>";
    
    // Battery Level Card
    html += "<div class='card'><h2>Battery Level</h2>";
    html += "<p>" + String(batteryVoltage, 2) + " V</p></div>";
    
    // Pump Status Card
    html += "<div class='card'><h2>Pump Status</h2>";
    html += "<p class='" + String(pumpActive ? "pump-on" : "pump-off") + "'>" + String(pumpActive ? "ACTIVE" : "OFF") + "</p></div>";
    
    html += "</div>";
    html += "<div class='btn-container'>";
    html += "<button onclick=\"fetch('/water').then(() => location.reload())\">💧 Manual Watering (10s)</button>";
    html += "</div>";
    html += "</div></body></html>";
    server.send(200, "text/html", html);
}

/**
 * @brief Handle manual watering request.
 */
void handleManualWatering() {
    if (!isWatering) {
        digitalWrite(RELAY_PIN, RELAY_ON);
        wateringStartTime = millis();
        isWatering = true;
        Serial.println("Watering started");
    }
    server.send(200, "text/plain", "Watering in progress or complete");
}

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

    // Control relay: turn ON when soil is DRY, OFF when WET
    if (soilDigital) {
        digitalWrite(RELAY_PIN, RELAY_ON);
        Serial.println("Relay: ON");
    } else {
        digitalWrite(RELAY_PIN, RELAY_OFF);
        Serial.println("Relay: OFF");
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
        digitalWrite(RELAY_PIN, RELAY_OFF);
        isWatering = false;
        Serial.println("Watering complete");
    }

    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(800);

}