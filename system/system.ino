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
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(SOLAR_VOLTAGE_PIN, INPUT);
    pinMode(BATTERY_LEVEL_PIN, INPUT);

    digitalWrite(RELAY_PIN, RELAY_OFF);

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
    // WIFI MAINTENANCE
    // =========================
    wifiReconnector.handle();
    setWifiLed(wifiReconnector.isConnected());

    server.handleClient();

    // =========================
    // SENSOR READS
    // =========================
    SoilMoistureSensor soilSensor(SOIL_MOISTURE_SENSOR_ANALOG_PIN);
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

    setPumpLed(pumpActive);

    Serial.println(shouldWater ? "Relay: ON" : "Relay: OFF");

    // =========================
    // WEB DATA UPDATE
    // =========================
    solarVoltage = solarVoltageReading;
    batteryVoltage = batteryVoltageReading;

    // =========================
    // PUSH BUTTON → STATUS LED
    // =========================
    // When the push button (GPIO 0 → VCC) is held, the built-in LED turns ON.
    // This is temporary — replace with the intended button action later.
    digitalWrite(LED_BUILTIN, isButtonPressed() ? HIGH : LOW);
    
}