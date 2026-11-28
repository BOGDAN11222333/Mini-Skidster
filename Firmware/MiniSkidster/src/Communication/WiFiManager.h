#pragma once

#include <Arduino.h>

#include "BLEManager.h"

// Manages the station-mode Wi-Fi connection using credentials provisioned over BLE.
// Connection attempts and reconnection are non-blocking.
class WiFiManager {
public:
  explicit WiFiManager(BLEManager& bleManager);

  void begin();
  void update();

  bool isConnected() const;

private:
  enum class State {
    WAITING_FOR_CREDENTIALS,
    CONNECTING,
    CONNECTED,
    WAITING_TO_RETRY,
    FAILED
  };

  void startConnection(bool isRetry);
  void scheduleRetry(const String& reason);
  void reportConnected();

  BLEManager& bleManager_;
  State state_ = State::WAITING_FOR_CREDENTIALS;
  unsigned long stateStartedAt_ = 0;
  uint8_t retryCount_ = 0;

  static constexpr unsigned long CONNECT_TIMEOUT_MS = 10000UL;
  static constexpr unsigned long RETRY_INTERVAL_MS = 5000UL;
  static constexpr uint8_t MAX_RETRIES = 5;
};
