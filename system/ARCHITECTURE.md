# Architecture Overview

This document describes the architecture and features of the ESP32-C3 based Watering System.

## System Overview

The Watering System is an automated IoT device designed to monitor soil moisture and manage a water pump based on predefined rules and user interaction. It features a web-based dashboard for real-time monitoring and manual control.

## Core Components

### 1. Hardware Abstraction Layer (HAL)
The system interacts with various hardware components through dedicated classes and functions:
- **Sensors**:
    - `SoilMoistureSensor`: Reads analog values to determine soil moisture levels (DRY, WET, FLOATING).
    - `VoltageSensor`: Measures solar panel and battery voltages.
- **Outputs**:
    - `PumpHandler`: Controls the water pump via a relay.
    - `Led`: Provides visual feedback for system status (WiFi, etc.).
    - `Button`: Handles physical user input for manual watering requests.
- **Connectivity**:
    - `WiFiReconnector`: Manages robust WiFi connection with automatic reconnection and exponential backoff.

### 2. System Logic & State Machine
The core logic resides in `system.ino` and manages the following states:
- **Auto Mode**: The system automatically triggers watering when the soil is dry and a cooldown period has elapsed.
- **Manual Mode**: The user can trigger a watering cycle via the web interface or a physical button.
- **Watering Cycle**: A timed window where the pump is active.
- **Cooldown Period**: A mandatory wait time between watering cycles to prevent over-watering.

### 3. Web Interface
A lightweight web server provides a dashboard for:
- **Real-time Monitoring**: Displays soil status, solar voltage, battery voltage, pump status, and auto-mode state.
- **Manual Control**: Buttons to trigger manual watering and toggle auto-mode.
- **Dynamic Updates**: The page auto-refreshes to provide near real-time data.

## Features

- **Automated Watering**: Intelligent scheduling based on moisture levels and cooldown timers.
- **Manual Override**: Physical button and web interface for immediate watering.
- **Power Monitoring**: Real-time tracking of solar and battery power.
- **Robust Connectivity**: Automatic WiFi reconnection to ensure the system remains reachable.
- **Responsive Web Dashboard**: Mobile-friendly UI for remote management.

## Data Flow

1. **Input**: Sensors (Soil, Voltage) and User Inputs (Button, Web Requests) are polled in the main loop.
2. **Processing**: The state machine evaluates inputs against current system state (Auto/Manual, Cooldown, Active Watering).
3. **Output**: The system drives the Pump, LEDs, and updates the Web Server state.
4. **Feedback**: The Web Server renders the current state into an HTML template for the user.
