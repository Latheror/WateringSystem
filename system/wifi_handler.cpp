#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "wifi_handler.h"

WebServer server(80);

/**
 * @brief Connects to a WiFi network and prints status to Serial.
 *
 * Attempts to connect to the specified WiFi network, printing progress
 * and result to the Serial monitor. Retries for up to 10 seconds.
 *
 * @param ssid     WiFi SSID (network name)
 * @param password WiFi password
 */
void connectToWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 50) {
        delay(200);
        Serial.print(".");
        retries++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi");
    }
}
