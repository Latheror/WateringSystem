#ifndef BATTERY_H
#define BATTERY_H

#include "settings.h"
#include "voltage_sensor.h"

/**
 * @brief Battery voltage measurement and pump safety policy.
 */
class Battery {
public:
    /**
     * @brief Initialize the battery ADC input.
     */
    void begin();

    /**
     * @brief Read the battery voltage in volts.
     */
    float read();

    /**
     * @brief Return true when the battery is below the pump safety threshold.
     */
    bool isLow(float voltage) const;

private:
    VoltageSensor _sensor{
        BATTERY_LEVEL_PIN,
        3.3f,
        4095,
        (BATTERY_DIVIDER_R1 + BATTERY_DIVIDER_R2) / BATTERY_DIVIDER_R1};
};

#endif // BATTERY_H
