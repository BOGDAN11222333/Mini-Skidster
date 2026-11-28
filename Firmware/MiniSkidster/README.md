# MiniSkidster Firmware

Firmware for the MiniSkidster RC skid-steer vehicle, based on the ESP32-C3 Super Mini.

## Development environment

- IDE: Arduino IDE 2.x
- Board package: ESP32 Arduino Core 3.x
- Board selection: **ESP32C3 Dev Module** (verify the selected board matches the installed ESP32-C3 Super Mini)
- Serial Monitor: 115200 baud

## Current development stage

**Stage 1.5 – Protocol and command expansion (development build 1.0.0-dev.5)**

The firmware initializes BLE advertising under the device name `MiniSkidster`. It exposes a read-only identification characteristic, separate write characteristics for Wi-Fi SSID and password, a command RX characteristic, and a response TX characteristic.

The command protocol currently validates these requests:

| Command | Parameters | Validation |
|---|---|---|
| `<PING>` | None | Connection test; returns `PONG` |
| `<ECHO|text>` | Text | Returns the supplied text |
| `<DRIVE|left|right>` | Each speed −100 to +100 | Signed percentage; negative means reverse |
| `<ARM|angle>` | 0–180 degrees | Arm servo request |
| `<BUCKET|angle>` | 0–180 degrees | Bucket servo request |
| `<STOP>` | None | Stop request |
| `<STATUS>` | None | Device status request |

Drive, arm, bucket, stop, and status commands currently return a `PARSED` acknowledgement only. No motor or servo hardware is initialized or activated in this stage. Servo limits are provisional protocol limits; safe mechanical calibration will be established before hardware control is implemented.

Malformed commands return `ERROR|INVALID_FORMAT`, out-of-range values return `ERROR|INVALID_PARAMETER`, and unsupported commands return `ERROR|UNKNOWN_COMMAND`.

Wi-Fi connection and hardware control are not implemented yet. Wi-Fi credentials, if received, are validated and stored in RAM only.

## Project architecture

The firmware follows a modular architecture adapted from the Oliver RC Car project:

- Communication/ – BLE initialization and command transport
- Protocol/ – command framing, parsing, and parameter validation
- Control/ – central vehicle command routing (planned)
- Modules/ – drive, arm, and bucket hardware modules (planned)
- Config/ – board, pin, and firmware configuration

Modules will be introduced incrementally and tested before integration.

## Build and test

1. Open `MiniSkidster.ino` in Arduino IDE.
2. Select the ESP32-C3 board configuration appropriate for the Super Mini.
3. Compile and upload to the ESP32-C3 Super Mini.
4. Open Serial Monitor at 115200 baud and confirm the startup banner reports firmware `1.0.0-dev.5`.
5. Test commands individually using the BLE app.

This development build is not a stable release until compilation, upload, and the documented tests have been completed successfully.
