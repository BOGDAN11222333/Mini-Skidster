# MiniSkidster Firmware V1.0.0 – Development Record

## Release status

**Status: Development – not yet stable**  
**Current build identifier:** 1.0.0-dev.1  
**Stage:** 1.1 – Firmware foundation  
**Target controller:** ESP32-C3 Super Mini

The V1.0.0 version number is reserved for the first stable firmware foundation. This document tracks its development. A stable release/tag will only be created after the acceptance tests below pass.

## Purpose

Establish a clean Arduino firmware project and startup sequence before implementing BLE provisioning, Wi-Fi control, protocol handling or vehicle hardware control.

## Implemented in this development build

- Main Arduino sketch and startup sequence.
- Central configuration header.
- Firmware version and board identification constants.
- Serial startup report at 115200 baud.
- Provisional GPIO assignments stored centrally for later stages.

## Not implemented

- BLE device discovery or Wi-Fi credential provisioning.
- Wi-Fi connection or control server.
- Application protocol and command processor.
- Drive motor control.
- Arm or bucket servo control.
- OTA updates or phone application integration.

## Architecture plan

The firmware will retain the logical separation used in the Oliver RC Car project:

1. Communication – BLE initialization/provisioning and Wi-Fi control transport.
2. Protocol – message framing, validation and parsing.
3. CommandProcessor – command dispatch.
4. VehicleControl – central vehicle-level logic.
5. Modules – drive, arm and bucket control.

## Configuration

| Item | Value |
|---|---|
| MCU board | ESP32-C3 Super Mini |
| Serial baud | 115200 |
| Left motor driver inputs | GPIO3, GPIO4 |
| Right motor driver inputs | GPIO5, GPIO6 |
| Arm servo signal | GPIO7 |
| Bucket servo signal | GPIO10 |

GPIO assignments are provisional software configuration based on the agreed initial pin plan. They have not yet been verified by physical wiring or peripheral tests. No GPIO outputs are activated by this development build.

## Acceptance tests for Stage 1.1

- [ ] Select the correct ESP32-C3 board target in Arduino IDE.
- [ ] Compile successfully.
- [ ] Upload successfully to the ESP32-C3 Super Mini.
- [ ] Open Serial Monitor at 115200 baud.
- [ ] Confirm firmware version, board name and chip information are printed.
- [ ] Confirm no motor or servo output is activated.

## Known limitations

This is a development build, not a stable release. It has not yet been compiled or uploaded as part of this recorded development step. GPIO assignments and board menu settings must be checked against the physical board before wiring.

## Next step

Complete Stage 1.1 compilation and upload testing. After successful verification, proceed to Stage 1.2 – BLE initialization and device discovery.

## Git history

The initial source files are committed together in the repository. The stable V1.0.0 tag will be created only after the acceptance tests pass.
