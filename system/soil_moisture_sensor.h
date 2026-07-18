#ifndef SOIL_MOISTURE_SENSOR_H
#define SOIL_MOISTURE_SENSOR_H

#include <Arduino.h>
#include "sensor.h"
#include "settings.h"

/**
 * @brief Implementation of a Soil Moisture Sensor.
 * Supports both analog and digital readings.
 */
enum class SoilStatus {
    UNKNOWN,
    DRY,
    WET,
    FLOATING
};

class SoilMoistureSensor : public Sensor {
public:
    /**
     * @brief Constructor for SoilMoistureSensor.
     * @param analogPin The analog pin for the sensor.
     * @param floatingThreshold The ADC threshold value for floating state.
     * @param wetThreshold The ADC threshold value for wet state.
     * @param analogMax Maximum ADC reading for the board (4095 for ESP32).
     * @param dryValue Raw value corresponding to dry soil.
     * @param wetValue Raw value corresponding to wet soil.
     */
    SoilMoistureSensor(int analogPin, int floatingThreshold = 100, int wetThreshold = SOIL_MOISTURE_THRESHOLD, int analogMax = 4095, int dryValue = 4095, int wetValue = 0);

    /**
     * @brief Reads the soil moisture level.
     * @return The moisture level as a percentage (0.0 to 100.0).
     */
    float read() override;

    /**
     * @brief Returns the soil status as an enum.
     * @return SoilStatus enum value.
     */
    SoilStatus getSoilStatus();

private:
    int _analogPin;
    int _floatingThreshold;
    int _wetThreshold;
    int _analogMax;
    int _dryValue;
    int _wetValue;
    SoilStatus _status;
};

#endif // SOIL_MOISTURE_SENSOR_H
