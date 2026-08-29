---
name: firmware-validation
description: Use when validating, compiling, preparing to flash, or investigating a WateringSystem ESP32-C3 firmware build.
---

# Firmware validation workflow

## Source validation

Run the `Build Firmware` VS Code task. It compiles the ESP32-C3 firmware without uploading. Use the direct `arduino-cli` compile command only as a fallback when VS Code tasks cannot be run.

Then inspect the diff for accidental GPIO, timing, polarity, credential, or generated-file changes.

## Device validation

Only flash after the user confirms deployment. Run the repository `Build & Flash Firmware` task, which owns the board, upload speed, port, and project path. Do not duplicate those task details in new instructions or documentation.

After flashing, validate at minimum:

- Wi-Fi connection and reconnect behavior.
- Soil readings, including disconnected/floating sensor behavior.
- Automatic watering start, duration, cooldown, and low-battery interruption.
- Manual watering mode and its intended safety behavior.
- Pump relay polarity and status LEDs while holding/releasing the button.
- Web dashboard state and controls.

Do not report a successful bench test when only compilation was performed. Do not include credentials in logs or reports.
