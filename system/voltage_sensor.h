#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include <Arduino.h>
#include "sensor.h"

/**
 * @brief Implementation of a voltage sensor.
 */
class VoltageSensor : public Sensor {
public:
    /**
     * @brief Constructor for voltage sensors.
     * @param analogPin ADC pin used to measure the voltage.
     * @param analogReference Reference voltage for ADC conversion.
     * @param analogMax Maximum ADC reading (4095 for ESP32).
     * @param scaleFactor Optional scale factor for voltage divider.
     */
    VoltageSensor(int analogPin, float analogReference = 3.3, int analogMax = 4095, float scaleFactor = 1.0);

    float read() override;
    String getStatus() override;

private:
    int _analogPin;
    float _analogReference;
    int _analogMax;
    float _scaleFactor;
};

#endif // VOLTAGE_SENSOR_H
