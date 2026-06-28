#include <Arduino.h>
#include "soil_moisture_sensor.h"

SoilMoistureSensor::SoilMoistureSensor(int analogPin, int digitalPin, int analogMax, int dryValue, int wetValue)
    : _analogPin(analogPin), _digitalPin(digitalPin), _analogMax(analogMax), _dryValue(dryValue), _wetValue(wetValue) {}

float SoilMoistureSensor::read() {
    int rawAnalog = analogRead(_analogPin);
    int normalizedAnalog = constrain(rawAnalog, 0, _analogMax);
    // Convert raw analog value to percentage using the configured dry/wet thresholds.
    float percentage = map(normalizedAnalog, _dryValue, _wetValue, 0, 100);
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
