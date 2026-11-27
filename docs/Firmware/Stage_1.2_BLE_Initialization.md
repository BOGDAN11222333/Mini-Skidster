# Stage 1.2 – BLE Initialization and Device Discovery

## Status
Development implementation committed. Compilation and hardware discovery tests are pending user verification.

## Purpose
Initialize Bluetooth Low Energy on the ESP32-C3 Super Mini and make MiniSkidster discoverable by a phone. This stage establishes the BLE server and a minimal GATT identification service without implementing Wi-Fi provisioning or vehicle commands.

## Implemented
- BLE device name: `MiniSkidster`.
- Custom GATT service UUID: `7b2e0001-6d3a-4f8c-9b21-5a7d3c8e1000`.
- Read-only device information characteristic UUID: `7b2e0002-6d3a-4f8c-9b21-5a7d3c8e1000`.
- Device information value includes the firmware build identifier.
- Serial messages report BLE initialization, advertising, connection and disconnection.
- Advertising restarts after a phone disconnects.

## Files
- `Firmware/MiniSkidster/MiniSkidster.ino` – starts BLE after printing the startup report.
- `Firmware/MiniSkidster/src/Communication/BLEManager.h` – BLE manager interface.
- `Firmware/MiniSkidster/src/Communication/BLEManager.cpp` – BLE server, GATT service, advertising and connection callbacks.
- `Firmware/MiniSkidster/src/Config/Config.h` – BLE name and UUID configuration.

## Test procedure
1. Open the repository's `Firmware/MiniSkidster/MiniSkidster.ino` sketch in Arduino IDE.
2. Select the ESP32-C3 board configuration used successfully for Stage 1.1.
3. Compile and upload.
4. Open Serial Monitor at 115200 baud and reset the board.
5. Confirm the log reports BLE initialization complete and advertising active.
6. Use a phone BLE scanner (not the phone's ordinary Bluetooth pairing screen) to scan for `MiniSkidster`.
7. Connect to the device and confirm the Serial Monitor reports the connection.
8. Disconnect and confirm advertising restarts and the device becomes discoverable again.

## Expected limitations
- No Wi-Fi credentials are accepted in this stage.
- No Wi-Fi connection or operational control server is started.
- No motor or servo GPIO is initialized.
- The phone application is not integrated yet.

## Acceptance status
- [ ] Compile
- [ ] Upload
- [ ] BLE device visible in a BLE scanner
- [ ] Connect/disconnect callbacks confirmed
- [ ] Advertising resumes after disconnect

Only mark Stage 1.2 complete after these tests pass on the physical ESP32-C3 Super Mini.
