#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "wifi_handler.h"
#include "settings.h"

/**
 * @brief Connects to a WiFi network and prints status to Serial.
 *
 * Starts a connection attempt without blocking the main loop. Connection
 * progress is handled by WiFiReconnector::handle().
 *
 * @param ssid     WiFi SSID (network name)
 * @param password WiFi password
 */
bool connectToWiFi(const char* ssid, const char* password) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("WiFi connection started");
    return WiFi.status() == WL_CONNECTED;
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
        Serial.println("[WiFiReconnector] Connection pending — retries will be handled in loop()");
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
        Serial.println("[WiFiReconnector] WiFi connection lost");
        _wasConnected = false;
        _lastAttemptMs = millis();

        // Cleanly tear down before reconnecting — don't call reconnect()
        // directly, it can race with the next begin().
        WiFi.disconnect(false, false); // wifioff=false, eraseap=false
        return;
    }

    // Already disconnected — check if it's time to retry
    unsigned long now = millis();
    if (now - _lastAttemptMs >= _currentDelayMs) {
        Serial.print("[WiFiReconnector] Retrying connection (delay=");
        Serial.print(_currentDelayMs);
        Serial.println("ms)...");

        // Ensure the driver is idle before reconfiguring/connecting.
        WiFi.disconnect(false, false);
        WiFi.begin(_ssid, _password);
        Serial.println("[WiFiReconnector] Reconnect started (non-blocking)");
        _currentDelayMs = min(_currentDelayMs * 2, (unsigned long)WIFI_RETRY_MAX_DELAY_MS);
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
