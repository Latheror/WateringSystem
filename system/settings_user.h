// User-specific settings (keep this file out of version control)
// Define your WiFi and MQTT credentials here.

#ifndef SETTINGS_USER_H
#define SETTINGS_USER_H

/**
 * @brief WiFi SSID for network connection.
 */
#define WIFI_SSID "CHANGE_ME"

/**
 * @brief WiFi password for network connection.
 */
#define WIFI_PASS "CHANGE_ME"

/**
 * @brief LAN IPv4 address of the MQTT broker.
 * Replace this placeholder with the Raspberry Pi address before deployment.
 */
#define MQTT_BROKER_HOST "CHANGE_ME"

/**
 * @brief MQTT username and password configured on EMQX.
 */
#define MQTT_USERNAME "CHANGE_ME"
#define MQTT_PASSWORD "CHANGE_ME"

#endif // SETTINGS_USER_H
