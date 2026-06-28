#include <Arduino.h>
#include "soil_moisture_sensor.h"

SoilMoistureSensor::SoilMoistureSensor(int analogPin, int digitalPin) 
    : _analogPin(analogPin), _digitalPin(digitalPin) {}

float SoilMoistureSensor::read() {
    int rawAnalog = analogRead(_analogPin);
    // Convert raw analog value (0-4095 for ESP32) to percentage (0-100)
    // Note: This assumes 4095 is dry and 0 is wet. Adjust if necessary.
    float percentage = map(rawAnalog, 4095, 0, 0, 100);
    return constrain(percentage, 0.0, 100.0);
}

String SoilMoistureSensor::getStatus() {
    // If digital pin is LOW, it usually means wet (depending on sensor type)
    // In the original system.ino, it was: soilDigital ? "DRY" : "WET"
    // This means HIGH = DRY, LOW = WET.
    SoilStatus status = getSoilStatus();
    if (status == SoilStatus::DRY) {
        return "DRY";
    } else if (status == SoilStatus::WET) {
        return "WET";
    } else {
        return "UNKNOWN";
    }
}

SoilStatus SoilMoistureSensor::getSoilStatus() {
    return digitalRead(_digitalPin) ? SoilStatus::DRY : SoilStatus::WET;
}
