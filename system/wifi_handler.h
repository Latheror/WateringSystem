#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>

/**
 * @file wifi_handler.h
 * @brief WiFi connection utility for ESP32C3 Watering System.
 */

/**
 * @brief Connects to a WiFi network and prints status to Serial.
 *
 * This function attempts to connect to the specified WiFi network.
 * It prints progress and result to the Serial monitor.
 *
 * @param ssid     WiFi SSID (network name)
 * @param password WiFi password
 */
void connectToWiFi(const char* ssid, const char* password);

#endif // WIFI_HANDLER_H
