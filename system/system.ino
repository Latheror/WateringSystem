// Define the LED pin for ESP32C3
#define LED_BUILTIN 8

#include "settings.h"
#include "wifi_handler.h"



void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  connectToWiFi(WIFI_SSID, WIFI_PASS);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on
  delay(100);                      // wait 100 ms
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off
  delay(100);                      // wait 100 ms
}
