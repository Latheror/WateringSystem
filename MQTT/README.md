# MQTT deployment

This directory is the single source of truth for the EMQX development and production broker deployment.

## Local setup

1. Copy `.env.example` to `.env`.
2. Set the MQTT and dashboard credentials locally. Do not commit `.env`.
3. Run `powershell -File scripts/broker.ps1 start`.
4. Check the broker with `powershell -File scripts/broker.ps1 status`.
5. Install MQTTX CLI once with `npm install --global mqttx-cli` for the
	authenticated publish/subscribe test.

The pinned image is `emqx/emqx:6.3.0`. MQTT is exposed on port `1883` and the dashboard on port `18083`. The setup uses EMQX's built-in password database, bootstrapped from an ignored runtime CSV generated from `.env`.

## Raspberry Pi setup

The Raspberry Pi must run 64-bit Raspberry Pi OS and report `aarch64` before deployment. Copy this directory, including the local `.env` and generated runtime bootstrap file, to the Pi without committing either file. Run `sh scripts/broker.sh start` from this directory.

The same image, credentials, topics, ports, and Compose configuration are used on both systems. MQTT remains limited to the trusted local network and is unencrypted in this first version.

## Operations

- `start`: prepare authentication data and start EMQX.
- `stop`: stop and remove the Compose container while keeping volumes.
- `restart`: restart the service.
- `status`: show Compose service status.
- `logs`: show the latest EMQX logs.
- `test`: prepare and start the service for the authenticated publish/subscribe test task.

Automated publish/subscribe checks use MQTTX CLI (`mqttx pub` and `mqttx sub`).
Mosquitto is not required or used by this project.

The state test publishes `examples/state-message.json` to `watering/state`:

```powershell
powershell -File scripts/test.ps1 -Topic watering/state -MessageFile examples/state-message.json
```

The command test publishes `examples/command-message.json` to
`watering/commands`:

```powershell
powershell -File scripts/test.ps1 -Topic watering/commands -MessageFile examples/command-message.json
```

Both payloads use the formats expected by the ESP32 firmware.

Passwords are never printed by the scripts.
