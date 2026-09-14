#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>
#include "soil_moisture_sensor.h"

using MqttCommandHandler = void (*)(bool autoMode, bool manualWatering);

/**
 * @brief Initialize the bounded MQTT client and command ledger.
 */
void initMqtt(MqttCommandHandler commandHandler);

/**
 * @brief Attempt one bounded broker connection.
 */
bool connectMqtt();

/**
 * @brief Process pending MQTT packets without blocking indefinitely.
 */
void handleMqtt();

/**
 * @brief Publish one coherent device state message.
 */
bool publishMqttState(float soilMoisture, SoilStatus soilStatus,
                      float solarVoltage, float batteryVoltage,
                      bool pumpActive, bool shouldWater, bool autoMode,
                      uint32_t remainingWaitingTimeSeconds, bool batteryLow,
                      bool wifiConnected);

/**
 * @brief Return whether the MQTT client is currently connected.
 */
bool isMqttConnected();

#endif // MQTT_HANDLER_H
