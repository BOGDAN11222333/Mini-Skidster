# MiniSkidster Firmware

Firmware for the MiniSkidster RC skid-steer vehicle, based on the ESP32-C3 Super Mini.

## Development environment

- IDE: Arduino IDE 2.x
- Board package: ESP32 Arduino Core 3.x
- Board selection: **ESP32C3 Dev Module** (verify the selected board matches the installed ESP32-C3 Super Mini)
- Serial Monitor: 115200 baud

## Current development stage

**Stage 2.0 – BLE Wi-Fi provisioning (development build 1.0.0-dev.7)**

The firmware initializes BLE advertising under the device name `MiniSkidster`. It exposes a read-only identification characteristic, separate write characteristics for Wi-Fi SSID and password, a command RX characteristic, and a response TX characteristic.

Wi-Fi provisioning is an explicit scan/select/connect sequence:

1. The phone sends `<WIFI_SCAN>` through the command RX characteristic.
2. The ESP32-C3 starts an asynchronous 2.4 GHz Wi-Fi scan. BLE remains active while the scan runs.
3. The ESP32-C3 sends `WIFI_SCAN_BEGIN|<count>`, one `WIFI_NETWORK|<JSON object>` message per network, then `WIFI_SCAN_END`. Each message uses the existing `@B...@E` BLE framing and 20-character notification chunking.
4. The phone displays the SSID and RSSI and lets the user select a network.
5. The phone writes the selected SSID to the SSID characteristic and the password to the password characteristic. Credentials are held in RAM only.
6. The phone sends `<WIFI_CONNECT>`. The ESP32-C3 then starts the station connection and reports its status and assigned IP address.

Each network JSON object contains:
- `ssid`: network name (JSON-escaped)
- `rssi`: received signal strength in dBm
- `secure`: `true` for a secured network, otherwise `false`

Provisioning responses include:
- `WIFI_SCAN_STARTED`, `WIFI_SCAN_BUSY`, `WIFI_SCAN_BEGIN|<count>`, `WIFI_NETWORK|<JSON>`, `WIFI_SCAN_END`
- `WIFI_CONNECT_REQUESTED`, `WIFI_CONNECTING`, `WIFI_CONNECTED|<IP address>`
- `WIFI_DISCONNECTED`, `WIFI_RETRYING|<attempt>/5`, `WIFI_FAILED|<reason>`

The vehicle command parser remains separate and unchanged. Existing commands such as `<PING>`, `<DRIVE|left|right>`, `<ARM|angle>`, and `<BUCKET|angle>` retain their existing validation-only behaviour. No motor or servo hardware is initialized or activated in this stage.

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

- Communication/ – BLE initialization, command transport, and Wi-Fi discovery/connection management
- Protocol – command framing, parsing, and parameter validation
- Control/ – central vehicle command routing (planned)
- Modules/ – drive, arm, and bucket hardware modules (planned)
- Config/ – board, pin, and firmware configuration

## Build and test

1. Open `MiniSkidster.ino` in Arduino IDE.
2. Select the ESP32-C3 board configuration appropriate for the Super Mini.
3. Compile and upload to the ESP32-C3 Super Mini.
4. Open Serial Monitor at 115200 baud.
5. Connect the phone over BLE.
6. Send `<WIFI_SCAN>` and confirm the scan-start response, network records, and scan-end response.
7. Select a network in the app, write its SSID and password to their existing BLE characteristics, then send `<WIFI_CONNECT>`.
8. Confirm the Serial Monitor reports a successful Wi-Fi connection and an IP address. Confirm the phone receives `WIFI_CONNECTED|<IP address>` if BLE notifications are enabled in the app.

The firmware implementation is not considered verified until it has been compiled, uploaded, and tested on the ESP32-C3 Super Mini.
