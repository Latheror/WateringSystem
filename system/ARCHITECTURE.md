# Watering System Architecture

This document describes the architecture and runtime behavior of the ESP32-C3
watering system.

## System Overview

The device monitors soil moisture and power conditions, controls a water pump,
and exposes a web dashboard for monitoring and manual control. The watering
state machine drives the pump; the physical button only controls temporary
status-LED visibility, to conserve battery.

## Core Components

### Sensors

- `SoilMoistureSensor`: Measures soil moisture and classifies it as `DRY`,
  `WET`, or `FLOATING`. `FLOATING` indicates a disconnected or invalid
  reading and is treated as a safety state (see below).
- `VoltageSensor`: Measures solar-panel voltage.
- `Battery`: Measures battery voltage and evaluates the low-battery condition.

### Inputs

- `Button`: Detects when the physical button is held down. Held state
  enables status-LED visibility for as long as it is held; it has no effect
  on watering.

### Outputs

- `PumpHandler`: Controls the water pump through the relay.
- `Led`: Controls four status indicators — Wi-Fi, pump, battery, and
  automatic-mode — as described in [Status LEDs](#status-leds).

### Connectivity

- `WiFiReconnector`: Maintains the Wi-Fi connection with automatic retries
  and exponential backoff.

## Operating Modes

The system operates in one of two modes, toggled from the web dashboard:

- **Automatic mode**: the watering state machine may start watering on its
  own when the automatic conditions are met.
- **Manual mode**: automatic watering is disabled. The user can force a
  watering cycle via a "water now" control on the web dashboard.

## Watering State Machine

The main control logic is implemented in `system.ino`.

- **Automatic watering** starts when all of the following hold:
    - the system is in automatic mode,
    - the soil reading is `DRY` (not `WET`, and not `FLOATING` — a
      `FLOATING` reading is treated as a safety condition and never triggers
      watering),
    - the cooldown period has elapsed,
    - the battery is above the pump safety threshold.
- **Manual watering** starts on request from the web dashboard while in
  manual mode, and **bypasses all other conditions** (battery threshold,
  cooldown, soil reading).
- **Watering cycle**: once started, the pump stays active for the configured
  duration, or until battery protection interrupts it.
- **Battery protection**: stops an *automatic* cycle early if the battery
  drops too low. Manual cycles are not subject to this check.
- **Cooldown**: after any automatic watering cycle, prevents another
  automatic cycle from starting until the configured interval has elapsed.
  Does not apply to manual watering.

## Status LEDs

There are four status LEDs, one per boolean system state: Wi-Fi connected,
pump active, battery low, and automatic mode enabled.

Each LED reflects its associated state, but is only physically lit while the
button is currently held down; as soon as the state is true **and** the
button is pressed, the LED turns on, and it turns off immediately on
release, regardless of the underlying state. This keeps the LEDs off by
default to conserve battery.

## Web Interface

The embedded web server provides:

- Real-time soil, solar-voltage, battery-voltage, pump, and mode
  information.
- A control to toggle between automatic and manual mode.
- A control to request manual watering (active only in manual mode).
- Periodic page updates for near-real-time status.

## Runtime Data Flow

1. The main loop reads the soil sensor, voltage sensors, button state, and
   Wi-Fi state, while the web server processes incoming requests.
2. The watering state machine evaluates the current mode, automatic
   conditions (soil, battery, cooldown), and any pending manual request.
3. The firmware drives the pump according to the watering state.
4. The firmware drives the status LEDs according to system state, gated by
   whether the button is currently held.
5. The latest sensor readings and system state are published through the
   web dashboard.