---
name: firmware-change
description: Use when implementing or reviewing a WateringSystem ESP32-C3 firmware change involving sensors, watering logic, pump control, LEDs, Wi-Fi, web routes, timing, or settings.
---

# Firmware change workflow

1. Read `system/ARCHITECTURE.md`, the target module, and its header before editing.
2. Identify the behavioral contract: inputs, outputs, timing, safety conditions, and whether the path is automatic or manual.
3. Make the smallest change that preserves existing module boundaries and configuration constants.
4. Check for edge cases involving invalid sensor readings, low battery, Wi-Fi loss, button release, repeated requests, and `millis()` rollover.
5. Update `system/ARCHITECTURE.md` if the externally visible behavior or safety policy changed.
6. Run the `Build Firmware` VS Code task to compile without uploading. Use the direct `arduino-cli` command only if the task runner is unavailable.

7. Report the changed files, compile result, and what still requires testing on real hardware.

Never upload firmware as part of this workflow unless the user explicitly requests flashing.
