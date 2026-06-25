#include <Arduino.h>
#include <WebServer.h>

#include "settings.h"
#include "wifi_handler.h"
#include "pump_handler.h"
#include "webapp.h"

WebServer server(80);

// =========================
// SYSTEM STATE (OWNED HERE)
// =========================
bool manualMode = false;
unsigned long wateringStartTime = 0;
bool pumpActive = false;

// =========================
// EVENT HANDLER (FROM WEB)
// =========================
void onManualWateringRequest() {
    manualMode = true;
    wateringStartTime = millis();

    Serial.println("Manual watering triggered (web event)");
}

// =========================
// SETUP
// =========================
void setup() {
    Serial.begin(115200);
    Serial.println("Start");

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(SOIL_MOISTURE_SENSOR_DIGITAL_PIN, INPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(SOLAR_VOLTAGE_PIN, INPUT);
    pinMode(BATTERY_LEVEL_PIN, INPUT);

    digitalWrite(RELAY_PIN, RELAY_OFF);

    connectToWiFi(WIFI_SSID, WIFI_PASS);

    initWebApp();

    Serial.println("Web server started on port 80");
}

// =========================
// LOOP (SYSTEM BRAIN)
// =========================
void loop() {

    server.handleClient();

    // =========================
    // SENSOR READS
    // =========================
    int soilAnalog = analogRead(SOIL_MOISTURE_SENSOR_ANALOG_PIN);
    int soilDigital = digitalRead(SOIL_MOISTURE_SENSOR_DIGITAL_PIN);

    int solarRaw = analogRead(SOLAR_VOLTAGE_PIN);
    int batteryRaw = analogRead(BATTERY_LEVEL_PIN);

    Serial.print("Soil Analog: ");
    Serial.print(soilAnalog);

    Serial.print(" | Soil Digital: ");
    Serial.print(soilDigital);

    Serial.print(" | Status: ");
    Serial.println(soilDigital ? "DRY" : "WET");

    // =========================
    // MANUAL MODE TIMEOUT
    // =========================
    if (manualMode && (millis() - wateringStartTime >= 10000)) {
        manualMode = false;
        Serial.println("Manual watering complete");
    }

    // =========================
    // DECISION LOGIC (ONLY HERE)
    // =========================
    bool shouldWater;

    if (manualMode) {
        shouldWater = true;
    } else {
        shouldWater = soilDigital;
    }

    // =========================
    // APPLY TO HARDWARE
    // =========================
    setPumpState(shouldWater);
    pumpActive = shouldWater;

    Serial.println(shouldWater ? "Relay: ON" : "Relay: OFF");

    // =========================
    // WEB DATA UPDATE
    // =========================
    solarVoltage = (solarRaw * 3.3) / 4095.0;
    batteryVoltage = (batteryRaw * 3.3) / 4095.0;

    // =========================
    // STATUS LED
    // =========================
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(800);
}