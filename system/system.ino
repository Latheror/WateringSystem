
#include "settings.h"
#include "wifi_handler.h"



void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); } // Wait for Serial (optional, for native USB)
  Serial.println("\n[BOOT] ESP32C3 Watering System starting...");
  pinMode(LED_BUILTIN, OUTPUT);
  connectToWiFi(WIFI_SSID, WIFI_PASS);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH); // turn the LED on
  delay(100);                      // wait 100 ms
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED off
  delay(100);                      // wait 100 ms
}
