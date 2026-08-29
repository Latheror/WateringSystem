# WateringSystem - Copilot Instructions

## Project context

WateringSystem is an Arduino firmware project for an ESP32-C3 automatic watering device. The firmware reads soil moisture, solar-panel voltage, and battery voltage; controls a pump relay; drives status LEDs; reconnects to Wi-Fi; and exposes an embedded web dashboard.

The main firmware lives in `system/`:

- `system/system.ino`: setup, main loop, and watering state machine.
- `system/settings.h`: shared hardware pins and timing thresholds.
- `system/settings_user.h`: local Wi-Fi credentials; never print, commit, or reproduce its values.
- `system/*_sensor.*`, `battery.*`, `pump_handler.*`, `led.*`, `button.*`, `wifi_handler.*`, and `webapp.*`: focused device modules.
- `system/ARCHITECTURE.md`: behavioral contract and runtime data flow.

The PCB and fabrication outputs are under `PCB/`. Treat electrical connections, GPIO assignments, voltage-divider values, active levels, and timing assumptions as hardware contracts.

## Working rules

- Read the relevant implementation and `system/ARCHITECTURE.md` before changing behavior.
- Keep changes small and localized. Preserve existing public APIs and module boundaries unless the task requires otherwise.
- Do not change GPIO assignments, ADC thresholds, relay polarity, sensor interpretation, or battery protection behavior without checking the schematic/PCB and documenting the impact.
- Preserve safety behavior: a floating or invalid soil reading must not start automatic watering, and automatic watering must respect battery protection and cooldown rules.
- Keep the main loop non-blocking except for the existing configured loop delay. Prefer `millis()`-based timing and state transitions over long delays.
- Avoid dynamic allocation and unnecessary `String` usage in long-running firmware paths.
- Never expose Wi-Fi credentials, tokens, or other secrets in source changes, logs, examples, or responses.
- Do not commit generated build artifacts or local credentials.
- Do not create commits or branches unless explicitly requested.

## Validation

Prefer the repository VS Code tasks instead of repeating build or upload commands:

- `Build Firmware`: compile for ESP32-C3 without uploading.
- `Build & Flash Firmware`: compile and upload to the configured ESP32-C3 on `COM14`; run only after the user explicitly requests deployment.
- `Find ESP IP`: locate the device on the network after deployment.

Use the task runner when available. The direct compile command is only a fallback when tasks cannot be run:

```text
arduino-cli compile --fqbn esp32:esp32:esp32c3 D:\Projets\WateringSystem\system
```

Before a physical deployment, inspect the diff and confirm the target board/port. Flashing is a separate, explicit action; do not upload automatically unless requested.

When reporting results, state what was changed, which validation command ran, and any hardware or runtime behavior that could not be tested without the device.