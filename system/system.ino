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
// WIFI
// =========================
WiFiReconnector wifiReconnector(WIFI_SSID, WIFI_PASS);

// =========================
// SYSTEM STATE
// =========================
bool manualRequestPending = false;
bool manualMode = false;
bool pumpActive = false;
bool autoMode = true;

bool wateringActive = false;

// Initialize so watering is immediately available after boot.
unsigned long lastWateringStartTime =
    (unsigned long)(0 - AUTO_WATERING_INTERVAL_MS);

// Countdown until another watering window may begin.
unsigned long remainingWaitingTimeMs = 0;

// =========================
// WEB EVENT HANDLER
// =========================
void onManualWateringRequest() {
    manualRequestPending = true;
    Serial.println("Manual watering requested");
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
// MAIN LOOP
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
    // WIFI
    // =========================
    wifiReconnector.handle();

    bool wifiConnected = wifiReconnector.isConnected();

    Serial.println(
        wifiConnected ? "wifiConnected: true"
                      : "wifiConnected: false");

    setWifiLed(wifiConnected && buttonPressed);
    setAutoModeLed(autoMode && buttonPressed);

    server.handleClient();

    // =========================
    // SENSORS
    // =========================
    SoilMoistureSensor soilSensor(
        SOIL_MOISTURE_SENSOR_ANALOG_PIN,
        SOIL_MOISTURE_FLOATING_THRESHOLD,
        SOIL_MOISTURE_THRESHOLD);

    VoltageSensor solarSensor(SOLAR_VOLTAGE_PIN);

    VoltageSensor batterySensor(
        BATTERY_LEVEL_PIN,
        3.3f,
        4095,
        (BATTERY_DIVIDER_R1 + BATTERY_DIVIDER_R2) / BATTERY_DIVIDER_R1);

    float soilMoisture = soilSensor.read();
    SoilStatus soilStatus = soilSensor.getSoilStatus();

    float solarVoltageReading = solarSensor.read();
    float batteryVoltageReading = batterySensor.read();

    Serial.print("Battery Voltage: ");
    Serial.println(batteryVoltageReading);

    setBatteryLowLed(false /* && buttonPressed */);

    Serial.print("Soil Moisture: ");
    Serial.print(soilMoisture);
    Serial.print("% | Status: ");

    switch (soilStatus) {
        case SoilStatus::DRY:
            Serial.println("DRY");
            break;

        case SoilStatus::WET:
            Serial.println("WET");
            break;

        case SoilStatus::FLOATING:
            Serial.println("FLOATING");
            break;

        default:
            Serial.println("UNKNOWN");
            break;
    }

    // =========================
    // WATERING CONTROL
    // =========================
    unsigned long currentTime = millis();
    unsigned long elapsedSinceStart =
        currentTime - lastWateringStartTime;

    bool wantsWater =
        manualRequestPending ||
        (soilStatus == SoilStatus::DRY);

    // Start a new watering window if requested and the interval has elapsed.
    if (!wateringActive &&
        wantsWater &&
        (elapsedSinceStart >= AUTO_WATERING_INTERVAL_MS)) {

        wateringActive = true;
        lastWateringStartTime = currentTime;
        elapsedSinceStart = 0;

        manualMode = manualRequestPending;
        manualRequestPending = false;

        Serial.println(
            manualMode
                ? "Manual watering started"
                : "Automatic watering started");
    }
    else if (manualRequestPending) {

        manualRequestPending = false;

        if (wateringActive) {
            Serial.println(
                "Manual request ignored: watering already active");
        } else {
            Serial.println(
                "Manual request ignored: waiting for interval");
        }
    }

    unsigned long remainingWateringTimeMs = 0;

    if (wateringActive) {

        if (elapsedSinceStart < AUTO_WATERING_DURATION_MS) {

            remainingWateringTimeMs =
                AUTO_WATERING_DURATION_MS - elapsedSinceStart;

        } else {

            wateringActive = false;
            manualMode = false;

            Serial.println("Watering window finished");
        }
    }

    if (!wateringActive &&
        elapsedSinceStart < AUTO_WATERING_INTERVAL_MS) {

        remainingWaitingTimeMs =
            AUTO_WATERING_INTERVAL_MS - elapsedSinceStart;

    } else {

        remainingWaitingTimeMs = 0;
    }

    bool shouldWater = (remainingWateringTimeMs > 0);

    // =========================
    // OUTPUTS
    // =========================
    Serial.println(
        shouldWater
            ? "shouldWater: TRUE"
            : "shouldWater: FALSE");

    setPumpState(shouldWater);

    pumpActive = shouldWater;
    setPumpLed(pumpActive && buttonPressed);

    Serial.println(
        shouldWater
            ? "Relay: ON"
            : "Relay: OFF");

    // =========================
    // WEB DATA
    // =========================
    solarVoltage = solarVoltageReading;
    batteryVoltage = batteryVoltageReading;

    // =========================
    // LOOP DELAY
    // =========================
    delay(LOOP_DELAY_MS);
}