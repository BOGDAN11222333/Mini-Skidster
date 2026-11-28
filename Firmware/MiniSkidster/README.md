# MiniSkidster Firmware

Firmware for the MiniSkidster RC skid-steer vehicle, based on the ESP32-C3 Super Mini.

## Development environment

- IDE: Arduino IDE 2.x
- Board package: ESP32 Arduino Core 3.x
- Board selection: **ESP32C3 Dev Module** (verify the selected board matches the installed ESP32-C3 Super Mini)
- Serial Monitor: 115200 baud

## Current development stage

**Stage 2.0 – Wi-Fi connection (development build 1.0.0-dev.6)**

The firmware initializes BLE advertising under the device name `MiniSkidster`. It exposes a read-only identification characteristic, separate write characteristics for Wi-Fi SSID and password, a command RX characteristic, and a response TX characteristic.

The phone provisions the Wi-Fi SSID and password through the existing BLE characteristics. The ESP32-C3 then connects to the configured Wi-Fi network in station mode. Credentials are held in RAM only; they are not saved to non-volatile storage.

The Wi-Fi manager uses a non-blocking state machine:
- Wait for both BLE credential characteristics to be written.
- Start the Wi-Fi connection and monitor its status.
- Apply a 10-second connection timeout.
- Retry a failed or lost connection up to five times, with a five-second interval.
- Report connection status and the assigned IP address over the existing BLE response channel.

Wi-Fi responses use the existing BLE framing `@B...@E` and include:
- `WIFI_CONNECTING`
- `WIFI_CONNECTED|<IP address>`
- `WIFI_DISCONNECTED`
- `WIFI_RETRYING|<attempt>/5`
- `WIFI_FAILED|<reason>`

No HTTP server is implemented yet. The command protocol continues to validate requests only; no motor or servo hardware is initialized or activated.

## Command protocol

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

## Project architecture

The firmware follows a modular architecture adapted from the Oliver RC Car project:

- Communication/ – BLE initialization, command transport, and Wi-Fi connection management
- Protocol/ – command framing, parsing, and parameter validation
- Control/ – central vehicle command routing (planned)
- Modules/ – drive, arm, and bucket hardware modules (planned)
- Config/ – board, pin, and firmware configuration

Modules will be introduced incrementally and tested before integration.

## Build and test

1. Open `MiniSkidster.ino` in Arduino IDE.
2. Select the ESP32-C3 board configuration appropriate for the Super Mini.
3. Compile and upload to the ESP32-C3 Super Mini.
4. Open Serial Monitor at 115200 baud.
5. Connect the phone over BLE and write the SSID and password to their existing characteristics.
6. Confirm the Serial Monitor reports a successful Wi-Fi connection and an IP address. Confirm the phone receives `WIFI_CONNECTED|<IP address>` if BLE notifications are enabled in the app.

This development build is not a stable release until compilation, upload, and the documented tests have been completed successfully.
