#include <Arduino.h>
#include "settings.h"
#include "pump_handler.h"

/**
 * @brief Sets the pump (relay) state.
 * @param active True to turn the pump ON, false to turn it OFF.
 */
void setPumpState(bool active) {
    digitalWrite(RELAY_PIN, !active);
}