# Watering System Project

## Overview
This project is an automated watering system for plants, featuring soil moisture monitoring, solar power management, and manual/automatic watering controls.

## Features
- **Soil Moisture Monitoring**: Real-time monitoring of soil moisture using both analog and digital sensors.
- **Automatic Watering**: Automatically activates the pump when soil moisture levels fall below a certain threshold.
- **Manual Watering**: Allows users to manually trigger a 10-second watering cycle via a web dashboard.
- **Power Management**: Monitors solar panel voltage and battery levels.
- **Web Dashboard**: A responsive web interface to view system status and control watering.

## Development Guidelines & Best Practices

### 1. Code Structure & Organization
- **Modularity**: Keep logic separated into different files (e.g., `wifi_handler.cpp`, `settings.h`).
- **Constants**: Use `settings.h` and `settings_user.h` for all hardware-specific configurations (pins, thresholds, WiFi credentials).
- **Non-blocking Code**: Avoid using `delay()` in the main loop where possible. Use `millis()` for timing-based operations (e.g., watering duration).

### 2. Hardware Abstraction
- **Relay Control**: Abstract relay operations to ensure consistent behavior (e.g., handling active-high vs. active-low modules).
- **Sensor Reading**: Standardize how sensors are read and processed before being used in logic.

### 3. Error Handling & Logging
- **Serial Debugging**: Use `Serial.println()` for critical state changes and errors.
- **Robustness**: Ensure the system can recover from WiFi disconnections or sensor failures.

### 4. Embedded C Best Practices
- **Memory Management**: Avoid dynamic memory allocation (`malloc`, `new`) to prevent memory fragmentation and leaks.
- **Data Types**: Use appropriate fixed-width integer types (e.g., `uint8_t`, `int32_t`) from `<stdint.h>`.
- **Naming Conventions**: Use clear, descriptive names for variables and functions.

## Project Structure
- `Board/`: Hardware design files.
- `Case/`: 3D model files for the enclosure.
- `Hardware/`: Component details and wiring diagrams.
- `system/`: Source code for the ESP32C3 microcontroller.
