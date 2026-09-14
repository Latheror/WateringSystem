#include "persistence.h"

#include <Preferences.h>

#include "settings.h"

namespace {
Preferences wateringWaitingTimePreferences;
RTC_DATA_ATTR uint32_t rtcWateringWaitingTimeRemainingMs = 0;
RTC_DATA_ATTR bool rtcWateringWaitingTimeActive = false;
RTC_DATA_ATTR uint32_t rtcWateringWaitingTimeMagic = 0;

constexpr uint32_t RTC_WATERING_WAITING_TIME_MAGIC = 0xC001D00D;
constexpr char WATERING_WAITING_TIME_NAMESPACE[] = "watering";

uint32_t wateringWaitingTimeRemainingMs = 0;
bool wateringWaitingTimePersistenceFailed = false;

bool persistWateringWaitingTime() {
    bool active = wateringWaitingTimeRemainingMs > 0;
    if (wateringWaitingTimePreferences.putBool("active", active) == 0 ||
        wateringWaitingTimePreferences.putULong("remaining", wateringWaitingTimeRemainingMs) == 0) {
        wateringWaitingTimePersistenceFailed = true;
        return false;
    }

    if (wateringWaitingTimePreferences.getBool("active", !active) != active ||
        wateringWaitingTimePreferences.getULong("remaining", UINT32_MAX) !=
            wateringWaitingTimeRemainingMs) {
        wateringWaitingTimePersistenceFailed = true;
        return false;
    }
    return true;
}
}

void initWateringWaitingTimePersistence() {
    if (!wateringWaitingTimePreferences.begin(WATERING_WAITING_TIME_NAMESPACE, false)) {
        wateringWaitingTimePersistenceFailed = true;
        return;
    }

    uint32_t nvsRemaining = wateringWaitingTimePreferences.getULong("remaining", 0);
    bool nvsActive = wateringWaitingTimePreferences.getBool("active", false);

    if (rtcWateringWaitingTimeMagic == RTC_WATERING_WAITING_TIME_MAGIC) {
        wateringWaitingTimeRemainingMs = rtcWateringWaitingTimeActive
                                            ? rtcWateringWaitingTimeRemainingMs
                                            : 0;
    } else {
        wateringWaitingTimeRemainingMs = nvsActive ? nvsRemaining : 0;
    }
}

uint32_t getRemainingWateringWaitingTimeMs() {
    return wateringWaitingTimeRemainingMs;
}

bool isWateringWaitingTimePersistenceHealthy() {
    return !wateringWaitingTimePersistenceFailed;
}

void advanceWateringWaitingTime(uint32_t elapsedMs) {
    wateringWaitingTimeRemainingMs = wateringWaitingTimeRemainingMs > elapsedMs
                                         ? wateringWaitingTimeRemainingMs - elapsedMs
                                         : 0;
}

bool startAutomaticWateringWaitingTime(uint32_t durationMs) {
    wateringWaitingTimeRemainingMs = durationMs;
    return persistWateringWaitingTime();
}

void prepareWateringWaitingTimeForDeepSleep(uint32_t sleepDurationMs) {
    rtcWateringWaitingTimeMagic = RTC_WATERING_WAITING_TIME_MAGIC;
    rtcWateringWaitingTimeActive = wateringWaitingTimeRemainingMs > 0;
    rtcWateringWaitingTimeRemainingMs = wateringWaitingTimeRemainingMs > sleepDurationMs
                                           ? wateringWaitingTimeRemainingMs - sleepDurationMs
                                           : 0;
    persistWateringWaitingTime();
}
