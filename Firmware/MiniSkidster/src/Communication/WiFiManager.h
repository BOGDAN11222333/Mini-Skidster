#pragma once

#include <Arduino.h>

#include "BLEManager.h"

// Manages Wi-Fi discovery and station connection using the existing app protocol.
class WiFiManager {
public:
  explicit WiFiManager(BLEManager& bleManager);

  void begin();
  void update();

  // Requests are queued here and processed from update(), outside BLE callbacks.
  String requestScan();
  String requestConnection(const String& ssid, const String& password);

  bool isConnected() const;

private:
  enum class State {
    WAITING_FOR_REQUEST,
    SCANNING,
    CONNECTING,
    CONNECTED,
    WAITING_TO_RETRY,
    FAILED
  };

  void updateScan();
  void reportScanResults(int16_t networkCount);
  void startConnection(bool isRetry);
  void scheduleRetry(const String& reason);
  void reportConnected();

  BLEManager& bleManager_;
  State state_ = State::WAITING_FOR_REQUEST;
  unsigned long stateStartedAt_ = 0;
  uint8_t retryCount_ = 0;
  bool connectionRequested_ = false;
  String ssid_;
  String password_;

  static constexpr unsigned long CONNECT_TIMEOUT_MS = 10000UL;
  static constexpr unsigned long RETRY_INTERVAL_MS = 5000UL;
  static constexpr uint8_t MAX_RETRIES = 5;
  static constexpr uint8_t MAX_REPORTED_NETWORKS = 20;
};
