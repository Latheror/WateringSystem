#include <Arduino.h>
#include "soil_moisture_sensor.h"

SoilMoistureSensor::SoilMoistureSensor(int analogPin, int threshold, int analogMax, int dryValue, int wetValue)
    : _analogPin(analogPin), _threshold(threshold), _analogMax(analogMax), _dryValue(dryValue), _wetValue(wetValue), _status(SoilStatus::UNKNOWN) {}

float SoilMoistureSensor::read() {
    int rawAnalog = analogRead(_analogPin);
    int normalizedAnalog = constrain(rawAnalog, 0, _analogMax);
    // Convert raw analog value to percentage using the configured dry/wet thresholds.
    float percentage = map(normalizedAnalog, _dryValue, _wetValue, 0, 100);

    // Update status based on threshold comparison
    if (rawAnalog > _threshold) {
        _status = SoilStatus::DRY;
    } else {
        _status = SoilStatus::WET;
    }

    return constrain(percentage, 0.0, 100.0);
}

String SoilMoistureSensor::getStatus() {
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
    return _status;
}
