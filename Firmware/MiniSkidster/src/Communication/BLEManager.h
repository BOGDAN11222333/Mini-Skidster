#pragma once

#include <Arduino.h>

// Handles MiniSkidster BLE initialization and GATT communication endpoints.
// Command parsing and Wi-Fi connection are implemented in later stages.
class BLEManager {
public:
  bool begin();
  bool isConnected() const;

  bool hasWiFiCredentials() const;
  String getProvisionedSSID() const;
  String getProvisionedPassword() const;
};
