#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <stdint.h>
#include <Arduino.h>

/**
 * @brief Initialize and restore the persisted automatic-watering wait time.
 */
void initWateringWaitingTimePersistence();

/**
 * @brief Return the remaining automatic-watering wait time in milliseconds.
 */
::uint32_t getRemainingWateringWaitingTimeMs();

/**
 * @brief Return whether watering wait-time persistence is healthy.
 */
bool isWateringWaitingTimePersistenceHealthy();

/**
 * @brief Advance the in-memory watering wait time by elapsed milliseconds.
 */
void advanceWateringWaitingTime(::uint32_t elapsedMs);

/**
 * @brief Start and persist a new automatic-watering wait time.
 * @return true when the value was persisted and verified.
 */
bool startAutomaticWateringWaitingTime(::uint32_t durationMs);

/**
 * @brief Save wait-time state and account for the upcoming deep-sleep interval.
 */
void prepareWateringWaitingTimeForDeepSleep(::uint32_t sleepDurationMs);

#endif // PERSISTENCE_H
