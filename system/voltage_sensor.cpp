#include <Arduino.h>
#include "voltage_sensor.h"

VoltageSensor::VoltageSensor(int analogPin, float analogReference, int analogMax, float scaleFactor)
    : _analogPin(analogPin), _analogReference(analogReference), _analogMax(analogMax), _scaleFactor(scaleFactor) {}

float VoltageSensor::read() {
    int rawAnalog = analogRead(_analogPin);
    float voltage = (rawAnalog * _analogReference / static_cast<float>(_analogMax)) * _scaleFactor;
    return voltage;
}

String VoltageSensor::getStatus() {
    return String(read(), 2) + " V";
}
