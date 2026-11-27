# Stage 1.3 – BLE Wi-Fi Credential Provisioning

- Firmware build: `1.0.0-dev.3`
- Board: ESP32-C3 Super Mini
- Status: Implemented in repository; compile and hardware validation pending

## Purpose

Receive the home Wi-Fi SSID and password from the phone over the existing BLE GATT service. This stage does not connect to Wi-Fi.

## BLE GATT service

Service UUID: `7b2e0001-6d3a-4f8c-9b21-5a7d3c8e1000`

| Characteristic | UUID suffix | Properties | Data |
|---|---|---|---|
| Device information | `0002` | Read | Firmware identification |
| Wi-Fi SSID | `0003` | Write | SSID, 1–32 bytes |
| Wi-Fi password | `0004` | Write | Password, 0–63 bytes |

Full UUIDs use the same base as the service:
- SSID: `7b2e0003-6d3a-4f8c-9b21-5a7d3c8e1000`
- Password: `7b2e0004-6d3a-4f8c-9b21-5a7d3c8e1000`

Write the SSID and password as UTF-8 text to their separate characteristics. An empty password is accepted for an open Wi-Fi network.

## Firmware behavior

- Rejects an empty SSID or an SSID longer than 32 bytes.
- Rejects a password longer than 63 bytes.
- Stores accepted values in RAM.
- Does not print the password to Serial.
- Reports when both fields have been received.
- Does not attempt a Wi-Fi connection; that belongs to Stage 1.4.
- Credentials are not saved to flash/NVS in this stage.

## Files

- `Firmware/MiniSkidster/src/Config/Config.h`
- `Firmware/MiniSkidster/src/Communication/BLEManager.h`
- `Firmware/MiniSkidster/src/Communication/BLEManager.cpp`
- `Firmware/MiniSkidster/MiniSkidster.ino`

## Security note

The provisioning characteristics use BLE GATT writes but this stage does not yet enforce BLE pairing/encryption. Test with a non-sensitive Wi-Fi network or test credentials until pairing/security is implemented. Do not share screenshots or logs containing actual credentials.

## Test procedure

1. Pull the latest repository changes in GitHub Desktop.
2. Open `Firmware/MiniSkidster/MiniSkidster.ino` in Arduino IDE.
3. Compile for the ESP32-C3 Super Mini.
4. Upload and open Serial Monitor at 115200 baud.
5. Connect with nRF Connect and verify the three characteristics are present.
6. Write a test SSID to the SSID characteristic.
7. Write a test password to the password characteristic.
8. Confirm Serial reports both values received, without displaying the password.
9. Confirm the device remains advertising/connected and does not start Wi-Fi.

## Acceptance criteria

- [ ] Compiles successfully.
- [ ] Upload succeeds.
- [ ] nRF Connect shows the information, SSID and password characteristics.
- [ ] Valid SSID is accepted.
- [ ] Valid password (or empty password for an open network) is accepted.
- [ ] Invalid lengths are rejected.
- [ ] Password content is never printed to Serial.
- [ ] No Wi-Fi connection is attempted in Stage 1.3.

## Next stage

Stage 1.4 – Connect to the supplied Wi-Fi network and report connection status and IP address over BLE.
