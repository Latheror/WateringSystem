#ifndef SOIL_MOISTURE_SENSOR_H
#define SOIL_MOISTURE_SENSOR_H

#include <Arduino.h>
#include "sensor.h"

/**
 * @brief Implementation of a Soil Moisture Sensor.
 * Supports both analog and digital readings.
 */
enum class SoilStatus {
    UNKNOWN,
    DRY,
    WET
};

class SoilMoistureSensor : public Sensor {
public:
    /**
     * @brief Constructor for SoilMoistureSensor.
     * @param analogPin The analog pin for the sensor.
     * @param digitalPin The digital pin for the sensor.
     */
    SoilMoistureSensor(int analogPin, int digitalPin);

    /**
     * @brief Reads the soil moisture level.
     * @return The moisture level as a percentage (0.0 to 100.0).
     */
    float read() override;

    /**
     * @brief Returns the status of the soil moisture.
     * @return "WET" or "DRY" based on the digital pin.
     */
    String getStatus() override;

    /**
     * @brief Returns the soil status as an enum.
     * @return SoilStatus enum value.
     */
    SoilStatus getSoilStatus();

private:
    int _analogPin;
    int _digitalPin;
};

#endif // SOIL_MOISTURE_SENSOR_H
