#ifndef WEBPAPP_H
#define WEBPAPP_H

#include <Arduino.h>
#include <WebServer.h>
#include "settings.h"
extern WebServer server;

/**
 * @file webapp.h
 * @brief Web application handler for the Watering System.
 */

// Global variables for web app
extern float solarVoltage;
extern float batteryVoltage;
extern bool pumpActive;

// Non-blocking variables for watering
extern unsigned long wateringStartTime;
extern bool isWatering;

/**
 * @brief Handle root page request.
 */
void handleRoot();

/**
 * @brief Handle manual watering request.
 */
void handleManualWatering();

/**
 * @brief Initialize the web server routes.
 */
void initWebApp();

#endif // WEBPAPP_H
