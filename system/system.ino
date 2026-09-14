#include <Arduino.h>

#include "settings.h"
#include "wifi_handler.h"
#include "mqtt_handler.h"
#include "pump_handler.h"
#include "soil_moisture_sensor.h"
#include "voltage_sensor.h"
#include "battery.h"
#include "led.h"
#include "button.h"
#include "persistence.h"
#include "deepsleep.h"

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

Battery battery;

// =========================
// SYSTEM STATE
// =========================
bool manualRequestPending = false;
bool manualMode = false;
bool pumpActive = false;
bool shouldWater = false;
bool autoMode = true;

bool wateringActive = false;
bool wasWifiConnected = false;
bool statePublishedForConnection = false;
unsigned long lastMqttAttemptTime = 0;
unsigned long wakeCycleStartTime = 0;
unsigned long lastWateringTickTime = 0;
bool wakeCycleEvaluated = false;

// Latest coherent sample published by the MQTT state publisher.
float solarVoltage = 0.0f;
float batteryVoltage = 0.0f;
float soilMoisture = 0.0f;
SoilStatus soilStatus = SoilStatus::UNKNOWN;

unsigned long lastWateringStartTime =
    (unsigned long)(0 - AUTO_WATERING_INTERVAL_MS);

// Countdown until another automatic watering window may begin.
unsigned long remainingWateringWaitingTimeMs = 0;

// =========================
// INPUT EVENT HANDLER
// =========================
void onManualWateringRequest() {
    manualRequestPending = true;
}

void onMqttCommand(bool requestedAutoMode, bool manualWatering) {
    autoMode = requestedAutoMode;
    if (manualWatering) {
        onManualWateringRequest();
    }
}

void prepareAndEnterDeepSleep() {
    setPumpState(false);

    const uint32_t sleepDurationMs = DEEP_SLEEP_WAKE_INTERVAL_SECONDS * 1000UL;
    prepareWateringWaitingTimeForDeepSleep(sleepDurationMs);
    enterDeepSleep();
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
    batteryVoltageReading = battery.read();

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
 * the soil is dry and the watering wait time has elapsed.
 * Ends the cycle once AUTO_WATERING_DURATION_MS has passed. Manual watering
 * is allowed even when the battery is low.
 *
 * @param currentTime Current time, in milliseconds (from millis()).
 * @param soilIsDry   Whether the soil sensor currently reports DRY.
 * @param batteryLow  Whether the battery is below the pump safety threshold.
 * @return true if the pump should be ON right now, false otherwise.
 */
bool updateWateringState(unsigned long currentTime, bool soilIsDry,
                         bool batteryLow) {
    unsigned long elapsedSinceStart = currentTime - lastWateringStartTime;
    unsigned long elapsedSinceTick = currentTime - lastWateringTickTime;
    lastWateringTickTime = currentTime;

    advanceWateringWaitingTime(elapsedSinceTick);
    remainingWateringWaitingTimeMs = getRemainingWateringWaitingTimeMs();

    // Low battery stops automatic watering, but not manual watering.
    if (batteryLow && wateringActive && !manualMode) {
        wateringActive = false;
        Serial.println("Watering stopped: battery too low");
        return false;
    }

    // Try to start a new watering cycle
    if (!wateringActive) {
        bool wateringWaitFinished = getRemainingWateringWaitingTimeMs() == 0;

        // Manual requests are intentional overrides and bypass the wait time.
        bool canStartManual = manualRequestPending;
        bool canStartAuto = autoMode && soilIsDry && wateringWaitFinished && !batteryLow;

        if (canStartManual ||
            (canStartAuto && isWateringWaitingTimePersistenceHealthy())) {
            if (canStartAuto) {
                if (!startAutomaticWateringWaitingTime(AUTO_WATERING_INTERVAL_MS)) {
                    return false;
                }
            }
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
            remainingWateringWaitingTimeMs = 0;
            return true;
        }

        wateringActive = false;
        manualMode = false;
        Serial.println("Watering window finished");
    }

    // Update the remaining automatic-watering wait time.
    remainingWateringWaitingTimeMs =
        (elapsedSinceStart < AUTO_WATERING_INTERVAL_MS)
            ? (AUTO_WATERING_INTERVAL_MS - elapsedSinceStart)
            : 0;

    return false;
}

/**
 * Arduino setup(). Initializes serial, pins, LEDs, the button, and WiFi.
 * Runs once on boot.
 */
void setup() {
    Serial.begin(115200);
    Serial.println("Start");

    // Drive the active-low relay to its safe state before enabling the output.
    digitalWrite(RELAY_PIN, HIGH);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(SOLAR_VOLTAGE_PIN, INPUT);
    battery.begin();

    setPumpState(false);

    initLeds();
    initButton();
    initMqtt(onMqttCommand);
    initWateringWaitingTimePersistence();

    bool wifiOk = wifiReconnector.begin();
    wakeCycleStartTime = millis();
    lastWateringTickTime = wakeCycleStartTime;
    setWifiLed(wifiOk);
}

/**
 * Arduino loop(). Reads inputs (button, WiFi, sensors), updates the
 * watering state machine, and drives outputs (pump, LEDs, relay).
 */
void loop() {
    // -------------------------
    // Button
    // -------------------------
    bool buttonPressed = isButtonPressed();

    // -------------------------
    // WiFi
    // -------------------------
    wifiReconnector.handle();
    bool wifiConnected = wifiReconnector.isConnected();

    if (wifiConnected && !wasWifiConnected) {
        WiFi.setSleep(true);
    }
    wasWifiConnected = wifiConnected;

    unsigned long currentTime = millis();
    if (wifiConnected && !isMqttConnected() &&
        currentTime - wakeCycleStartTime < MQTT_WAKE_CYCLE_DEADLINE_MS &&
        currentTime - lastMqttAttemptTime >= MQTT_CONNECTION_TIMEOUT_MS) {
        lastMqttAttemptTime = currentTime;
        if (connectMqtt()) {
            statePublishedForConnection = false;
        }
    }
    handleMqtt();

    // Status LEDs are only visible while the button is held.
    setWifiLed(wifiConnected && buttonPressed);
    setAutoModeLed(autoMode && buttonPressed);

    // -------------------------
    // Sensors
    // -------------------------
    float soilMoistureReading, solarVoltageReading, batteryVoltageReading;
    SoilStatus soilStatusReading;
    readAndLogSensors(soilMoistureReading, soilStatusReading, solarVoltageReading,
                       batteryVoltageReading);

    // Publish the same coherent sample that drives the controller and MQTT.
    soilMoisture = soilMoistureReading;
    soilStatus = soilStatusReading;
    solarVoltage = solarVoltageReading;
    batteryVoltage = batteryVoltageReading;

    bool batteryLow = battery.isLow(batteryVoltageReading);
    setBatteryLowLed(batteryLow && buttonPressed);

    // -------------------------
    // Watering control
    // -------------------------
    bool soilIsDry = (soilStatusReading == SoilStatus::DRY);

    shouldWater = updateWateringState(currentTime, soilIsDry, batteryLow);
    Serial.println(shouldWater ? "shouldWater: TRUE" : "shouldWater: FALSE");

    setPumpState(shouldWater);
    pumpActive = shouldWater;
    setPumpLed(pumpActive && buttonPressed);

    if (isMqttConnected() && !statePublishedForConnection) {
        publishMqttState(
            soilMoisture, soilStatus, solarVoltage, batteryVoltage,
            pumpActive, shouldWater, autoMode,
            remainingWateringWaitingTimeMs / 1000UL, batteryLow, wifiConnected);
        statePublishedForConnection = true;
    }

    wakeCycleEvaluated = true;
    if (wakeCycleEvaluated &&
        currentTime - wakeCycleStartTime >= MQTT_WAKE_CYCLE_DEADLINE_MS &&
        !wateringActive) {
        prepareAndEnterDeepSleep();
    }

    // -------------------------
    // Loop delay
    // -------------------------
    delay(LOOP_DELAY_MS);
}