#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

/**
 * @brief Base class for all sensors in the Watering System.
 * Provides a common interface for reading sensor values.
 */
class Sensor {
public:
    virtual ~Sensor() {}
    /**
     * @brief Reads the sensor value and returns it.
     * @return The sensor value (e.g., moisture level, voltage).
     */
    virtual float read() = 0;
    /**
     * @brief Returns a human-readable status or description of the sensor.
     * @return A string describing the sensor's current state.
     */
    virtual String getStatus() = 0;
};

#endif // SENSOR_H
