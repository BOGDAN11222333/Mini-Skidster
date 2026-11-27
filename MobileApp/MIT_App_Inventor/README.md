# MiniSkidster – MIT App Inventor Project

This folder is reserved for the MiniSkidster phone application developed with MIT App Inventor.

## Files to store here

- `MiniSkidster.aia` – editable MIT App Inventor source project (upload/export from MIT App Inventor).
- `README.md` – app version history, screen descriptions, BLE/Wi-Fi communication details and test notes.
- `assets/` – images and other project assets, if needed.

The compiled Android APK may be stored separately under `releases/` when a tested app build is available.

## Current app development stage

The app will follow the agreed connection flow:

1. Discover and connect to the MiniSkidster using BLE.
2. Read device information and firmware version.
3. Send the home Wi-Fi SSID and password to the ESP32 over BLE.
4. Receive Wi-Fi connection status and IP address.
5. Use Wi-Fi for normal vehicle control.

The editable `.aia` project has not been uploaded yet.
