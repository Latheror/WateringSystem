#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

/**
 * @file wifi_handler.h
 * @brief WiFi connection utility for ESP32C3 Watering System.
 */

/**
 * @brief Connects to a WiFi network.
 *
 * Attempts to connect to the specified WiFi network, printing
 * progress to Serial. The WiFi LED is also updated accordingly.
 *
 * @param ssid     WiFi SSID (network name)
 * @param password WiFi password
 * @return true if connected successfully, false otherwise
 */
bool connectToWiFi(const char* ssid, const char* password);

/**
 * @brief Web server instance.
 */
extern WebServer server;

/**
 * @brief Automatic WiFi reconnector with exponential backoff.
 *
 * Handles reconnection transparently: call begin() once in setup(),
 * then call handle() regularly from loop(). When WiFi drops, it
 * automatically retries with exponentially increasing delays
 * (capped at WIFI_RETRY_MAX_DELAY_MS). On successful reconnect the
 * backoff resets to the base delay.
 */
class WiFiReconnector {
public:
    /**
     * @brief Construct a WiFiReconnector.
     * @param ssid     WiFi SSID
     * @param password WiFi password
     */
    WiFiReconnector(const char* ssid, const char* password);

    /**
    * @brief Start the initial connection attempt without blocking.
    *
    * Call once in setup(), then call handle() from loop().
     *
     * @return true if connected, false otherwise.
     */
    bool begin();

    /**
     * @brief Periodic maintenance — call from loop().
     *
     * Checks WiFi status. If disconnected and enough time has
     * elapsed since the last attempt, triggers a reconnection.
     * On success the backoff resets; on failure the delay doubles.
     */
    void handle();

    /**
     * @brief Check current WiFi connectivity.
     * @return true if WiFi is connected.
     */
    static bool isConnected();

    /**
     * @brief Manually reset the backoff timer (e.g. after user action).
     */
    void reset();

private:
    const char* _ssid;
    const char* _password;
    unsigned long _lastAttemptMs;
    unsigned long _currentDelayMs;
    bool _wasConnected;
};

#endif // WIFI_HANDLER_H
