#include "deepsleep.h"

#include <Arduino.h>
#include <esp_sleep.h>

#include "settings.h"

void enterDeepSleep() {
    esp_sleep_enable_timer_wakeup(
        static_cast<uint64_t>(DEEP_SLEEP_WAKE_INTERVAL_SECONDS) * 1000000ULL);
    esp_deep_sleep_start();
}

