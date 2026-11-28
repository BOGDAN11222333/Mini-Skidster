# MiniSkidster Firmware

Firmware for the MiniSkidster RC skid-steer vehicle, based on the ESP32-C3 Super Mini.

## Development environment

- IDE: Arduino IDE 2.x
- Board package: ESP32 Arduino Core 3.x
- Board selection: **ESP32C3 Dev Module** (verify the selected board matches the installed ESP32-C3 Super Mini)
- Serial Monitor: 115200 baud

## Current development stage

**Stage 2.0 – BLE Wi-Fi provisioning (development build 1.0.0-dev.8)**

The firmware uses the existing MIT App Inventor BLE interface and message protocol. The app remains responsible for displaying the Wi-Fi list, selecting an SSID, and collecting the password. The ESP32 sends scan results in the exact message sequence the app already parses.

### Wi-Fi provisioning sequence

1. The app sends `<SCAN_WIFI>` to the BLE command RX characteristic.
2. The ESP32 starts an asynchronous 2.4 GHz Wi-Fi scan.
3. When the scan completes, the ESP32 sends:
   - `@BWIFI_LIST_START@E` – the app clears its current Wi-Fi list.
   - One `@BWIFI:<SSID>@E` message for each discovered network – the app adds each SSID to its existing list.
   - `@BWIFI_LIST_END@E` – the app completes/displays the list.
4. You select the desired SSID in the app and enter its password.
5. The app sends `<WIFI_CONNECT:ssid,password>` through the same BLE command RX characteristic.
6. The ESP32 connects and sends `WIFI_CONNECTING`, followed by `WIFI_CONNECTED|<IP address>` on success, or a failure/retry message.

All ESP-to-phone messages retain the existing `@B...@E` framing and 20-character BLE notification chunking. Scan result names deliberately match the app's existing parser; no JSON format or app-side protocol change is required.

The combined credential command uses the first comma as the SSID/password separator. Passwords may contain additional commas. As with the existing app protocol, an SSID containing a comma is not supported by this command format.

## BLE identifiers

- Device name: `MiniSkidster`
- Service: `7b2e0001-6d3a-4f8c-9b21-5a7d3c8e1000`
- Device information READ: `7b2e0002-6d3a-4f8c-9b21-5a7d3c8e1000`
- Wi-Fi SSID WRITE (legacy characteristic): `7b2e0003-6d3a-4f8c-9b21-5a7d3c8e1000`
- Wi-Fi password WRITE (legacy characteristic): `7b2e0004-6d3a-4f8c-9b21-5a7d3c8e1000`
- Command RX WRITE: `7b2e0005-6d3a-4f8c-9b21-5a7d3c8e1000`
- Response TX READ/NOTIFY: `7b2e0006-6d3a-4f8c-9b21-5a7d3c8e1000`

The app's current combined `<WIFI_CONNECT:ssid,password>` command is handled by the RX characteristic. The separate SSID/password characteristics remain available for compatibility but are not used by this app flow.

## Vehicle command protocol

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

Malformed commands return a validation error, and unsupported commands return `ERROR|UNKNOWN_COMMAND`.

## Build and test

1. Open `MiniSkidster.ino` in Arduino IDE.
2. Select the ESP32-C3 board configuration appropriate for the Super Mini.
3. Compile and upload to the ESP32-C3 Super Mini.
4. Open Serial Monitor at 115200 baud and connect the phone over BLE.
5. Trigger the app's Wi-Fi scan. Confirm the app's existing list is cleared and repopulated with the received SSIDs.
6. Select a network, enter its password, and connect from the app.
7. Confirm the Serial Monitor reports a successful connection and IP address, and the app receives `WIFI_CONNECTED|<IP address>`.

The firmware implementation is not considered verified until it has been compiled, uploaded, and tested on the ESP32-C3 Super Mini.
