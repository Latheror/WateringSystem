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
// SENSORS
// =========================
SoilMoistureSensor soilSensor(SOIL_MOISTURE_SENSOR_ANALOG_PIN);

VoltageSensor solarSensor(
    SOLAR_VOLTAGE_PIN,
    3.3f,
    4095,
    (SOLAR_DIVIDER_R1 + SOLAR_DIVIDER_R2) / SOLAR_DIVIDER_R1);

VoltageSensor batterySensor(
    BATTERY_LEVEL_PIN,
    3.3f,
    4095,
    (BATTERY_DIVIDER_R1 + BATTERY_DIVIDER_R2) / BATTERY_DIVIDER_R1);

// =========================
// SYSTEM STATE
// =========================
bool manualRequestPending = false;
bool manualMode = false;
bool pumpActive = false;
bool autoMode = true;

bool wateringActive = false;
bool wasWifiConnected = false;

// Initialize so watering is immediately available after boot.
unsigned long lastWateringStartTime =
    (unsigned long)(0 - AUTO_WATERING_INTERVAL_MS);

// Countdown until another watering window may begin.
// Read by webapp.cpp to display the remaining wait time.
unsigned long remainingWaitingTimeMs = 0;

// =========================
// WEB EVENT HANDLER
// =========================
/**
 * Web server callback invoked when the user requests manual watering.
 * Sets a flag that is picked up by updateWateringState() on the next loop.
 */
void onManualWateringRequest() {
    manualRequestPending = true;
    Serial.println("Manual watering requested");
}

// =========================
// HELPERS
// =========================

/**
 * Reads the soil moisture, solar voltage, and battery voltage sensors,
 * and logs a compact one-line summary to Serial.
 *
 * @param[out] soilMoisture         Soil moisture reading, in percent.
 * @param[out] soilStatus           Classified soil status (DRY/WET/FLOATING).
 * @param[out] solarVoltageReading  Solar panel voltage, in volts.
 * @param[out] batteryVoltageReading Battery voltage, in volts.
 */
void readAndLogSensors(float &soilMoisture, SoilStatus &soilStatus,
                        float &solarVoltageReading,
                        float &batteryVoltageReading) {
    soilMoisture = soilSensor.read();
    soilStatus = soilSensor.getSoilStatus();

    solarVoltageReading = solarSensor.read();
    batteryVoltageReading = batterySensor.read();

    const char *soilStatusStr = "UNKNOWN";
    switch (soilStatus) {
        case SoilStatus::DRY:      soilStatusStr = "DRY";      break;
        case SoilStatus::WET:      soilStatusStr = "WET";      break;
        case SoilStatus::FLOATING: soilStatusStr = "FLOATING"; break;
        default: break;
    }

    Serial.printf(
        "Battery: %.2fV | Solar: %.2fV | Soil: %.1f%% (%s)\n",
        batteryVoltageReading, solarVoltageReading, soilMoisture,
        soilStatusStr);
}

/**
 * Advances the watering state machine by one tick.
 *
 * Starts a new watering cycle if either a manual request is pending, or
 * the soil is dry and the cooldown since the last cycle has elapsed.
 * Ends the cycle once AUTO_WATERING_DURATION_MS has passed.
 *
 * @param currentTime Current time, in milliseconds (from millis()).
 * @param soilIsDry   Whether the soil sensor currently reports DRY.
 * @return true if the pump should be ON right now, false otherwise.
 */
bool updateWateringState(unsigned long currentTime, bool soilIsDry) {
    unsigned long elapsedSinceStart = currentTime - lastWateringStartTime;

    // Try to start a new watering cycle
    if (!wateringActive) {
        bool cooldownFinished =
            elapsedSinceStart >= AUTO_WATERING_INTERVAL_MS;

        bool canStartManual = manualRequestPending;
        bool canStartAuto = autoMode && soilIsDry && cooldownFinished;

        if (canStartManual || canStartAuto) {
            wateringActive = true;
            manualMode = canStartManual;
            manualRequestPending = false;

            lastWateringStartTime = currentTime;
            elapsedSinceStart = 0;

            Serial.println(manualMode
                                ? "Manual watering started"
                                : "Automatic watering started");
        }
    }

    // Run / finish the active watering window
    if (wateringActive) {
        if (elapsedSinceStart < AUTO_WATERING_DURATION_MS) {
            remainingWaitingTimeMs = 0;
            return true;
        }

        wateringActive = false;
        manualMode = false;
        Serial.println("Watering window finished");
    }

    // In cooldown: update the countdown shown on the web page
    remainingWaitingTimeMs =
        (elapsedSinceStart < AUTO_WATERING_INTERVAL_MS)
            ? (AUTO_WATERING_INTERVAL_MS - elapsedSinceStart)
            : 0;

    return false;
}

/**
 * Arduino setup(). Initializes serial, pins, LEDs, the button, WiFi,
 * and the web app. Runs once on boot.
 */
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

/**
 * Arduino loop(). Reads inputs (button, WiFi, sensors), updates the
 * watering state machine, drives outputs (pump, LEDs, relay), and
 * publishes the latest readings for the web app.
 */
void loop() {
    // -------------------------
    // Button
    // -------------------------
    bool buttonPressed = isButtonPressed();
    if (buttonPressed) {
        Serial.println("Button pressed");
    }

    // -------------------------
    // WiFi
    // -------------------------
    wifiReconnector.handle();
    bool wifiConnected = wifiReconnector.isConnected();

    if (wifiConnected && !wasWifiConnected) {
        WiFi.setSleep(true);
    }
    wasWifiConnected = wifiConnected;

    Serial.println(wifiConnected ? "wifiConnected: true"
                                 : "wifiConnected: false");

    setWifiLed(wifiConnected && buttonPressed);
    setAutoModeLed(autoMode && buttonPressed);

    server.handleClient();

    // -------------------------
    // Sensors
    // -------------------------
    float soilMoisture, solarVoltageReading, batteryVoltageReading;
    SoilStatus soilStatus;
    readAndLogSensors(soilMoisture, soilStatus, solarVoltageReading,
                       batteryVoltageReading);

    setBatteryLowLed(batteryVoltageReading < BATTERY_LOW_THRESHOLD && buttonPressed);

    // -------------------------
    // Watering control
    // -------------------------
    unsigned long currentTime = millis();
    bool soilIsDry = (soilStatus == SoilStatus::DRY);

    bool shouldWater = updateWateringState(currentTime, soilIsDry);

    Serial.println(shouldWater ? "shouldWater: TRUE" : "shouldWater: FALSE");

    setPumpState(shouldWater);
    pumpActive = shouldWater;
    setPumpLed(pumpActive && buttonPressed);

    Serial.println(shouldWater ? "Relay: ON" : "Relay: OFF");

    // -------------------------
    // Web data
    // -------------------------
    solarVoltage = solarVoltageReading;
    batteryVoltage = batteryVoltageReading;

    // -------------------------
    // Loop delay
    // -------------------------
    delay(LOOP_DELAY_MS);
}