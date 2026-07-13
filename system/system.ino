#include <Arduino.h>
#include <WebServer.h>

#include "settings.h"
#include "wifi_handler.h"
#include "pump_handler.h"
#include "webapp.h"
#include "soil_moisture_sensor.h"
#include "voltage_sensor.h"
#include "led.h"
#include "button.h"

WebServer server(80);

// =========================
// WIFI RECONNECTOR (GLOBAL)
// =========================
WiFiReconnector wifiReconnector(WIFI_SSID, WIFI_PASS);

// =========================
// SYSTEM STATE (OWNED HERE)
// =========================
// NOTE: manual and automatic watering share a single timer/budget
// so the pump can only be ON for AUTO_WATERING_DURATION_MS every
// AUTO_WATERING_INTERVAL_MS, regardless of what triggered it.
bool manualRequestPending = false;   // set by web event, consumed in loop()
bool manualMode = false;             // reporting only: was the current window manually triggered?
bool pumpActive = false;

bool wateringActive = false;         // true while pump is inside its ON window (manual or auto)
unsigned long lastWateringStartTime = (unsigned long)(0 - AUTO_WATERING_INTERVAL_MS);

// =========================
// EVENT HANDLER (FROM WEB)
// =========================
void onManualWateringRequest() {
    manualRequestPending = true;
    Serial.println("Manual watering requested (web event)");
}

// =========================
// SETUP
// =========================
void setup() {
    Serial.begin(115200);
    Serial.println("Start");

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(SOLAR_VOLTAGE_PIN, INPUT);
    pinMode(BATTERY_LEVEL_PIN, INPUT);

    setPumpState(false);

    initLeds();
    initButton();

    bool wifiOk = wifiReconnector.begin();
    setWifiLed(wifiOk);

    initWebApp();

    Serial.println("Web server started on port 80");
}

// =========================
// LOOP (SYSTEM BRAIN)
// =========================
void loop() {

    // =========================
    // BUTTON
    // =========================
    bool buttonPressed = isButtonPressed();
    if (buttonPressed) {
        Serial.println("Button pressed");
    }

    // =========================
    // WIFI MAINTENANCE
    // =========================
    wifiReconnector.handle();
    bool wifiConnected = wifiReconnector.isConnected();
    Serial.println(wifiConnected ? "wifiConnected: true" : "wifiConnected: false");
    setWifiLed(wifiConnected && buttonPressed);

    server.handleClient();

    // =========================
    // SENSOR READS
    // =========================
    SoilMoistureSensor soilSensor(SOIL_MOISTURE_SENSOR_ANALOG_PIN, SOIL_MOISTURE_FLOATING_THRESHOLD, SOIL_MOISTURE_THRESHOLD);
    VoltageSensor solarSensor(SOLAR_VOLTAGE_PIN);
    VoltageSensor batterySensor(BATTERY_LEVEL_PIN, 3.3f, 4095, (BATTERY_DIVIDER_R1 + BATTERY_DIVIDER_R2) / BATTERY_DIVIDER_R1);

    float soilMoisture = soilSensor.read();
    SoilStatus soilStatus = soilSensor.getSoilStatus();
    float solarVoltageReading = solarSensor.read();
    float batteryVoltageReading = batterySensor.read();
    Serial.print("Battery Voltage: ");
    Serial.println(batteryVoltageReading);
    setBatteryLowLed(false /*&& buttonPressed*/);

    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisture);
    Serial.print("% | Status: ");
    if (soilStatus == SoilStatus::DRY) {
        Serial.println("DRY");
    } else if (soilStatus == SoilStatus::WET) {
        Serial.println("WET");
    } else if (soilStatus == SoilStatus::FLOATING) {
        Serial.println("FLOATING");
    } else {
        Serial.println("UNKNOWN");
    }

    // =========================
    // DECISION LOGIC (ONLY HERE)
    // =========================
    unsigned long currentTime = millis();
    bool wantsWater = manualRequestPending || (soilStatus == SoilStatus::DRY);

    // Only allow a NEW watering window to start if the shared budget
    // window (AUTO_WATERING_INTERVAL_MS) has elapsed since the last one.
    if (!wateringActive && wantsWater &&
        (currentTime - lastWateringStartTime >= AUTO_WATERING_INTERVAL_MS)) {
        wateringActive = true;
        lastWateringStartTime = currentTime;
        manualMode = manualRequestPending; // just for reporting which triggered it
        manualRequestPending = false;      // consume the request either way
        Serial.println(manualMode ? "Manual watering started" : "Automatic watering started");
    } else if (manualRequestPending && wateringActive) {
        // Already inside a watering window (budget in use) - drop the request
        manualRequestPending = false;
        Serial.println("Manual request ignored: watering budget already in use");
    } else if (manualRequestPending) {
        // Wanted to water manually but budget window hasn't elapsed yet - drop it
        manualRequestPending = false;
        Serial.println("Manual request ignored: watering budget not yet available");
    }

    bool shouldWater = false;
    if (wateringActive) {
        if (currentTime - lastWateringStartTime < AUTO_WATERING_DURATION_MS) {
            shouldWater = true;
        } else {
            shouldWater = false;
            wateringActive = false;
            manualMode = false;
            Serial.println("Watering window finished");
        }
    }

    // =========================
    // APPLY TO HARDWARE
    // =========================
    Serial.println(shouldWater ? "shouldWater: TRUE" : "shouldWater: FALSE");
    setPumpState(shouldWater);

    pumpActive = shouldWater;
    setPumpLed(pumpActive && buttonPressed);

    Serial.println(shouldWater ? "Relay: ON" : "Relay: OFF");

    // =========================
    // WEB DATA UPDATE
    // =========================
    solarVoltage = solarVoltageReading;
    batteryVoltage = batteryVoltageReading;

    // =========================
    // LOOP DELAY
    // =========================
    delay(LOOP_DELAY_MS);

}