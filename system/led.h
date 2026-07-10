#ifndef LED_H
#define LED_H

/**
 * @file led.h
 * @brief LED control functions for status indicators.
 */

/**
 * @brief Initialize all LED pins as outputs.
 */
void initLeds();

/**
 * @brief Set the state of the WiFi connected LED.
 * @param connected True to turn ON (WiFi connected), false to turn OFF.
 */
void setWifiLed(bool connected);

/**
 * @brief Set the state of the pump active LED.
 * @param active True to turn ON (pump running), false to turn OFF.
 */
void setPumpLed(bool active);

/**
 * @brief Set the state of the battery low LED.
 * @param low True to turn ON (battery low warning), false to turn OFF.
 */
void setBatteryLowLed(bool low);

#endif // LED_H
