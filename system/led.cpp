#include <Arduino.h>
#include "settings.h"
#include "led.h"

/**
 * @brief Initialize all LED pins as outputs and turn them off.
 */
void initLeds() {
    pinMode(LED_PIN_WIFI_CONNECTED, OUTPUT);
    pinMode(LED_PIN_PUMP_ACTIVE, OUTPUT);
    pinMode(LED_PIN_BATTERY_LOW, OUTPUT);
    pinMode(LED_PIN_AUTO_MODE, OUTPUT);

    // The board LEDs are active-low: HIGH is the electrical OFF level.
    digitalWrite(LED_PIN_WIFI_CONNECTED, HIGH);
    digitalWrite(LED_PIN_PUMP_ACTIVE, HIGH);
    digitalWrite(LED_PIN_BATTERY_LOW, HIGH);
    digitalWrite(LED_PIN_AUTO_MODE, HIGH);
}

/**
 * @brief Set the state of the WiFi connected LED.
 */
void setWifiLed(bool connected) {
    digitalWrite(LED_PIN_WIFI_CONNECTED, connected ? LOW : HIGH);
}

/**
 * @brief Set the state of the pump active LED.
 */
void setPumpLed(bool active) {
    digitalWrite(LED_PIN_PUMP_ACTIVE, active ? LOW : HIGH);
}

/**
 * @brief Set the state of the battery low LED.
 * @param low True to turn ON (battery low warning), false to turn OFF.
 */
void setBatteryLowLed(bool low) {
    digitalWrite(LED_PIN_BATTERY_LOW, low ? LOW : HIGH);
}

/**
 * @brief Set the state of the auto mode LED.
 * @param auto_mode True to turn ON (auto mode active), false to turn OFF.
 */
void setAutoModeLed(bool auto_mode) {
    digitalWrite(LED_PIN_AUTO_MODE, auto_mode ? LOW : HIGH);
}
