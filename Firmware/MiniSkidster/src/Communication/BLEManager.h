#pragma once

#include <Arduino.h>

// Handles BLE initialization, identification and Wi-Fi credential reception.
// Wi-Fi connection and status reporting are implemented in Stage 1.4.
class BLEManager {
public:
  bool begin();
  bool isConnected() const;

  bool hasWiFiCredentials() const;
  String getProvisionedSSID() const;
  String getProvisionedPassword() const;
};
