#include <Arduino.h>
#include <WebServer.h>

#include "settings.h"
#include "wifi_handler.h"
#include "pump_handler.h"
#include "webapp.h"
#include "soil_moisture_sensor.h"
#include "voltage_sensor.h"

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
    SoilMoistureSensor soilSensor(SOIL_MOISTURE_SENSOR_ANALOG_PIN, SOIL_MOISTURE_SENSOR_DIGITAL_PIN);
    VoltageSensor solarSensor(SOLAR_VOLTAGE_PIN);
    VoltageSensor batterySensor(BATTERY_LEVEL_PIN);

    float soilMoisture = soilSensor.read();
    SoilStatus soilStatus = soilSensor.getSoilStatus();
    float solarVoltageReading = solarSensor.read();
    float batteryVoltageReading = batterySensor.read();

    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisture);
    Serial.print("% | Status: ");
    if (soilStatus == SoilStatus::DRY) {
        Serial.println("DRY");
    } else if (soilStatus == SoilStatus::WET) {
        Serial.println("WET");
    } else {
        Serial.println("UNKNOWN");
    }

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
        // Use the digital status from the sensor object
        shouldWater = (soilStatus == SoilStatus::DRY);
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
    solarVoltage = solarVoltageReading;
    batteryVoltage = batteryVoltageReading;

    // =========================
    // STATUS LED
    // =========================
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(800);
}