#include <Arduino.h>
#include "soil_moisture_sensor.h"

SoilMoistureSensor::SoilMoistureSensor(int analogPin, int floatingThreshold, int wetThreshold, int analogMax, int dryValue, int wetValue)
    : _analogPin(analogPin), _floatingThreshold(floatingThreshold), _wetThreshold(wetThreshold), _analogMax(analogMax), _dryValue(dryValue), _wetValue(wetValue), _status(SoilStatus::UNKNOWN) {}

float SoilMoistureSensor::read() {
    int rawAnalog = analogRead(_analogPin);
    int normalizedAnalog = constrain(rawAnalog, 0, _analogMax);
    // Convert raw analog value to percentage using the configured dry/wet thresholds.
    float percentage = map(normalizedAnalog, _dryValue, _wetValue, 0, 100);

    // Update status based on threshold comparison
    // FLOATING: ADC reading is very low (sensor not properly in soil) or moisture is 0%
    if (rawAnalog < _floatingThreshold || percentage == 0.0) {
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
