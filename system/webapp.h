#ifndef WEBAPP_H
#define WEBAPP_H

#include <Arduino.h>
#include <WebServer.h>
#include "settings.h"

extern WebServer server;

/**
 * @file webapp.h
 * @brief Web application handler for the Watering System.
 */

// =========================
// READ-ONLY DISPLAY STATE
// (updated by system.ino)
// =========================

extern float solarVoltage;
extern float batteryVoltage;
extern bool pumpActive;
extern bool autoMode;
extern unsigned long remainingWaitingTimeMs;

/**
 * @brief Handle root page request.
 */
void handleRoot();

/**
 * @brief Handle manual watering request.
 * Sends event to system.ino
 */
void handleManualWatering();

/**
 * @brief Handle auto mode toggle request.
 * Sends event to system.ino
 */
void handleToggleAutoMode();

/**
 * @brief Handle manual watering request from web.
 * Signals system.ino to start manual watering.
 */
void onManualWateringRequest();

/**
 * @brief Initialize the web server routes.
 */
void initWebApp();


#endif // WEBAPP_H