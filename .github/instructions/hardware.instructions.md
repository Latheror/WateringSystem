---
name: WateringSystem hardware constraints
description: Apply when editing KiCad files, hardware documentation, pin mappings, sensor calibration, or electrical behavior.
applyTo: "{PCB/**/*.kicad_*,PCB/**/*.kicad_sym,PCB/**/*.kicad_mod,Hardware/**/*,Board/**/*,system/settings.h,system/*sensor*,system/battery.*}"
---

# Hardware constraints

- Treat the schematic and PCB as the source of truth for connectivity and pin usage.
- Before changing a GPIO, verify the corresponding schematic net, PCB pad, board variant, and ESP32-C3 boot implications.
- Before changing an ADC conversion or threshold, verify the resistor-divider values, expected voltage range, ADC attenuation/configuration, and sensor wiring.
- Check relay active level and power-path behavior before changing pump control logic.
- For KiCad changes, preserve project-library references and run the appropriate ERC/DRC or connectivity check when available.
- Do not claim hardware validation from compilation alone. Clearly separate source-level validation from bench testing.
- Never place Wi-Fi credentials or other secrets in hardware documentation or generated reports.
