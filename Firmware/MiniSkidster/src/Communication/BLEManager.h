#pragma once

#include <Arduino.h>

// Handles BLE initialization, advertising and connection status.
// Wi-Fi provisioning and application commands are added in later stages.
class BLEManager {
public:
  bool begin();
  bool isConnected() const;
};
