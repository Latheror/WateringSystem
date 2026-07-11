#include <Arduino.h>
#include "settings.h"
#include "button.h"

// =========================
// INTERNAL STATE
// =========================
static bool _lastStableState = LOW;   // debounced state
static bool _lastRawState    = LOW;   // previous raw read
static unsigned long _lastChangeMs = 0;
static bool _pendingPress = false;    // one-shot flag

// =========================
// INIT
// =========================
void initButton() {
    pinMode(BUTTON_PIN, INPUT_PULLDOWN);
    _lastStableState = digitalRead(BUTTON_PIN);
    _lastRawState    = _lastStableState;
    _lastChangeMs    = millis();
}

// =========================
// DEBOUNCE ENGINE
// =========================
static void _updateDebounce() {
    bool raw = digitalRead(BUTTON_PIN);

    // State changed → reset debounce timer
    if (raw != _lastRawState) {
        _lastRawState = raw;
        _lastChangeMs = millis();
        return;
    }

    // State stable long enough → accept it
    if (raw != _lastStableState && (millis() - _lastChangeMs >= BUTTON_DEBOUNCE_MS)) {
        // Detect rising edge (LOW → HIGH) for one-shot
        if (raw == HIGH && _lastStableState == LOW) {
            _pendingPress = true;
        }
        _lastStableState = raw;
    }
}

// =========================
// PUBLIC API
// =========================
bool isButtonPressed() {
    _updateDebounce();
    return _lastStableState == HIGH;
}

bool wasButtonPressed() {
    _updateDebounce();
    if (_pendingPress) {
        _pendingPress = false;
        return true;
    }
    return false;
}
