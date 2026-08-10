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

## PCB

The KiCad design files for the watering system are located in `PCB/`. These files are read and edited with KiCad 10.

### Design files

- `PCB/WateringSystem_PCB/WateringSystem_PCB.kicad_pro`: KiCad project file. It stores the KiCad project configuration and references the schematic and PCB layout.
- `PCB/WateringSystem_PCB/WateringSystem_PCB.kicad_sch`: KiCad schematic file. It describes the components, symbols, power rails, nets, and logical connections.
- `PCB/WateringSystem_PCB/WateringSystem_PCB.kicad_pcb`: KiCad PCB board file. It contains the board outline, footprints, copper tracks, vias, zones, and silkscreen.
- `PCB/WateringSystem_PCB/fp-lib-table`: KiCad footprint-library table. It tells KiCad where to find the custom and external footprint libraries used by the board.

### Custom libraries

- `PCB/ESP32-C3_SuperMini.kicad_sym`: Custom KiCad symbol-library file containing the ESP32-C3 SuperMini symbol used by the schematic.
- `PCB/General.kicad_sym`: Custom KiCad symbol-library file containing additional project-specific schematic symbols.
- `PCB/WateringSystem_PCB/ESP32-C3_SuperMini.pretty/ESP32-C3_SuperMini.kicad_mod`: Custom KiCad footprint file for the ESP32-C3 SuperMini module.

### Konnect configuration

- `PCB/WateringSystem_PCB/.konnect/project.json`: Project-specific KiCad/Konnect configuration, including design rules and fabrication preferences. Keep it versioned when those shared project settings are expected to be reproducible.

Generated Gerbers, drill files, ERC reports, backups, lock files, KiCad local preferences, and history files are intentionally excluded by `.gitignore`. They can be regenerated from the versioned design sources when needed.
