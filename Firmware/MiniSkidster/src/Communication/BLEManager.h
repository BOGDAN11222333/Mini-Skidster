#pragma once

#include <Arduino.h>

class WiFiManager;

// Handles MiniSkidster BLE initialization and GATT communication endpoints.
class BLEManager {
public:
  bool begin();
  bool isConnected() const;

  void setWiFiManager(WiFiManager* manager);

  bool hasWiFiCredentials() const;
  String getProvisionedSSID() const;
  String getProvisionedPassword() const;

  void sendTestNotification();
  void sendResponse(const String& payload);
};
