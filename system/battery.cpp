#include <Arduino.h>
#include "battery.h"
#include "settings.h"

void Battery::begin() {
    pinMode(BATTERY_LEVEL_PIN, INPUT);
}

float Battery::read() {
    return _sensor.read();
}

bool Battery::isLow(float voltage) const {
    return voltage < BATTERY_LOW_THRESHOLD;
}
