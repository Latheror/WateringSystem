
#include <Arduino.h>
#include "settings.h"
#include "wifi_handler.h"

/**
 * @file system.ino
 * @brief Main entry point for ESP32C3 Watering System.
 */

/**
 * @brief Arduino setup function. Initializes Serial, LED, and WiFi.
 */
void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  pinMode(LED_BUILTIN, OUTPUT);
  connectToWiFi(WIFI_SSID, WIFI_PASS);
}

/**
 * @brief Arduino main loop. Blinks LED and prints to Serial.
 */
void loop() {
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on
  delay(100);                      // wait 100 ms
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off
  delay(100);                      // wait 100 ms
}
