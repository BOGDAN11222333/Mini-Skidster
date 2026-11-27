# MiniSkidster Firmware

Firmware for the MiniSkidster RC skid-steer vehicle, based on the ESP32-C3 Super Mini.

## Development environment

- IDE: Arduino IDE 2.x
- Board package: ESP32 Arduino Core 3.x
- Board selection: **ESP32C3 Dev Module** (verify the selected board matches the installed ESP32-C3 Super Mini)
- Serial Monitor: 115200 baud

## Current development stage

**Stage 1.3 – BLE Wi-Fi credential provisioning (development build 1.0.0-dev.3)**

The current development build initializes BLE advertising under the device name `MiniSkidster`. It exposes a read-only identification characteristic plus separate write characteristics for Wi-Fi SSID and password. Received credentials are validated and stored in RAM only; they are not printed to Serial and are not persisted. The ESP32 does not connect to Wi-Fi in this stage. Wi-Fi connection/status handling is Stage 1.4. Motors and servos are not implemented.

## Project architecture

The firmware will follow the modular architecture established for the Oliver RC Car, adapted to the ESP32-C3:

- Communication/ – BLE initialization/provisioning and Wi-Fi communication
- Protocol/ – message framing and command parsing
- Control/ – central vehicle command routing
- Modules/ – drive, arm and bucket hardware modules
- Config/ – board, pin and firmware configuration

Modules will be introduced incrementally and tested before being integrated.

## Build and test

1. Open MiniSkidster.ino in Arduino IDE.
2. Select the ESP32-C3 board configuration appropriate for the Super Mini.
3. Compile and upload to the ESP32-C3 Super Mini.
4. Open Serial Monitor at 115200 baud and press reset if necessary.
5. Confirm the startup banner reports the firmware version and ESP32-C3 chip information.

This development build is not a stable release until compilation, upload and the documented tests have been completed successfully.
