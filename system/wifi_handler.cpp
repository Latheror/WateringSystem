#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "wifi_handler.h"
#include "settings.h"

/**
 * @brief Connects to a WiFi network and prints status to Serial.
 *
 * Attempts to connect to the specified WiFi network, printing progress
 * and result to the Serial monitor. Retries for up to 10 seconds.
 *
 * @param ssid     WiFi SSID (network name)
 * @param password WiFi password
 */
bool connectToWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 50) {
        delay(200);
        Serial.print(".");
        retries++;
    }
    bool connected = WiFi.status() == WL_CONNECTED;
    if (connected) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFailed to connect to WiFi");
    }
    return connected;
}

// =========================
// WiFiReconnector
// =========================

WiFiReconnector::WiFiReconnector(const char* ssid, const char* password)
    : _ssid(ssid)
    , _password(password)
    , _lastAttemptMs(0)
    , _currentDelayMs(WIFI_RETRY_BASE_DELAY_MS)
    , _wasConnected(false) {}

bool WiFiReconnector::begin() {
    Serial.println("[WiFiReconnector] Initial connection attempt...");
    bool ok = connectToWiFi(_ssid, _password);
    if (ok) {
        _wasConnected = true;
        _currentDelayMs = WIFI_RETRY_BASE_DELAY_MS;
    } else {
        _wasConnected = false;
        _lastAttemptMs = millis();
        Serial.println("[WiFiReconnector] Failed — retries will be handled in loop()");
    }
    return ok;
}

void WiFiReconnector::handle() {
    wl_status_t status = WiFi.status();

    if (status == WL_CONNECTED) {
        if (!_wasConnected) {
            Serial.println("[WiFiReconnector] Reconnected successfully!");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            _wasConnected = true;
            _currentDelayMs = WIFI_RETRY_BASE_DELAY_MS;
        }
        return;
    }

    // Not connected
    if (_wasConnected) {
        // Just dropped — log once
        Serial.println("[WiFiReconnector] WiFi connection lost");
        _wasConnected = false;
        _lastAttemptMs = millis();
        // Attempt immediate reconnect
        Serial.println("[WiFiReconnector] Attempting immediate reconnect...");
        WiFi.reconnect();
        return;
    }

    // Already disconnected — check if it's time to retry
    unsigned long now = millis();
    if (now - _lastAttemptMs >= _currentDelayMs) {
        Serial.print("[WiFiReconnector] Retrying connection (delay=");
        Serial.print(_currentDelayMs);
        Serial.println("ms)...");
        WiFi.begin(_ssid, _password);

        // Wait a few seconds to see if it connects
        int retries = 0;
        while (WiFi.status() != WL_CONNECTED && retries < 25) {
            delay(200);
            retries++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("[WiFiReconnector] Reconnected successfully!");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            _wasConnected = true;
            _currentDelayMs = WIFI_RETRY_BASE_DELAY_MS;
        } else {
            Serial.println("[WiFiReconnector] Reconnect attempt failed");
            _currentDelayMs = min(_currentDelayMs * 2, (unsigned long)WIFI_RETRY_MAX_DELAY_MS);
            Serial.print("[WiFiReconnector] Next retry in ");
            Serial.print(_currentDelayMs / 1000);
            Serial.println(" seconds");
        }
        _lastAttemptMs = millis();
    }
}

bool WiFiReconnector::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiReconnector::reset() {
    _currentDelayMs = WIFI_RETRY_BASE_DELAY_MS;
    _lastAttemptMs = 0;
}
