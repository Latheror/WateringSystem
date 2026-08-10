#include <Arduino.h>
#include "soil_moisture_sensor.h"

SoilMoistureSensor::SoilMoistureSensor(int analogPin, int floatingThreshold, int wetThreshold, int analogMax, int dryValue, int wetValue)
    : _analogPin(analogPin), _floatingThreshold(floatingThreshold), _wetThreshold(wetThreshold), _analogMax(analogMax), _dryValue(dryValue), _wetValue(wetValue), _status(SoilStatus::UNKNOWN) {}

float SoilMoistureSensor::read() {
    int rawAnalog = analogRead(_analogPin);
    int normalizedAnalog = constrain(rawAnalog, 0, _analogMax);
    // Convert raw analog value to percentage using the configured dry/wet thresholds.
    const float range = static_cast<float>(_wetValue - _dryValue);
    float percentage = (range == 0.0f)
        ? 0.0f
        : (static_cast<float>(normalizedAnalog - _dryValue) / range) * 100.0f;

    // Update status based on threshold comparison
    // FLOATING: ADC reading is very low (sensor not properly in soil).
    if (rawAnalog < _floatingThreshold) {
        _status = SoilStatus::FLOATING;
    } else if (rawAnalog > _wetThreshold) {
        _status = SoilStatus::DRY;
    } else {
        _status = SoilStatus::WET;
    }

    return constrain(percentage, 0.0, 100.0);
}

SoilStatus SoilMoistureSensor::getSoilStatus() {
    return _status;
}
