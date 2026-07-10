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

    digitalWrite(LED_PIN_WIFI_CONNECTED, LOW);
    digitalWrite(LED_PIN_PUMP_ACTIVE, LOW);
    digitalWrite(LED_PIN_BATTERY_LOW, LOW);
}

/**
 * @brief Set the state of the WiFi connected LED.
 */
void setWifiLed(bool connected) {
    digitalWrite(LED_PIN_WIFI_CONNECTED, connected ? HIGH : LOW);
}

/**
 * @brief Set the state of the pump active LED.
 */
void setPumpLed(bool active) {
    digitalWrite(LED_PIN_PUMP_ACTIVE, active ? HIGH : LOW);
}

/**
 * @brief Set the state of the battery low LED.
 */
void setBatteryLowLed(bool low) {
    digitalWrite(LED_PIN_BATTERY_LOW, low ? HIGH : LOW);
}
