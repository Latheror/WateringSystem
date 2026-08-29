---
name: WateringSystem firmware conventions
description: Apply when editing Arduino, C++, or header files in the ESP32-C3 firmware.
applyTo: "system/**/*.{ino,cpp,h}"
---

# Firmware conventions

- Keep each module's implementation in its matching `.cpp` file and declarations in its matching `.h` file.
- Use the existing class and function names unless an API change is necessary.
- Include `settings.h` for project configuration rather than duplicating pin numbers or timing constants.
- Use fixed-width integer types where a width matters, and use `uint32_t` for millisecond timestamps.
- Handle `millis()` rollover-safe comparisons with unsigned subtraction.
- Keep `loop()` responsive: do not add blocking waits for sensor reads, Wi-Fi retries, pump cycles, or web requests.
- Preserve the distinction between automatic and manual watering. Manual watering may bypass automatic conditions only where the existing architecture specifies it.
- Treat invalid, floating, or disconnected sensor readings as safe inputs; never turn them into an automatic watering request.
- Avoid logging secrets and avoid adding verbose serial output to battery-sensitive paths.
- Update `ARCHITECTURE.md` when a user-visible state transition or safety rule changes.
