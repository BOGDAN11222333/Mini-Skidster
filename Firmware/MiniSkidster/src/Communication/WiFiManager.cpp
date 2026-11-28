#include "WiFiManager.h"

#include <WiFi.h>

WiFiManager::WiFiManager(BLEManager& bleManager)
    : bleManager_(bleManager) {}

void WiFiManager::begin() {
  Serial.println();
  Serial.println("[WIFI] Initializing Wi-Fi station mode...");

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(false);
  WiFi.disconnect();

  state_ = State::WAITING_FOR_CREDENTIALS;
  stateStartedAt_ = millis();
  retryCount_ = 0;

  Serial.println("[WIFI] Ready. Waiting for BLE-provisioned credentials.");
}

void WiFiManager::update() {
  switch (state_) {
    case State::WAITING_FOR_CREDENTIALS:
      if (bleManager_.hasWiFiCredentials()) {
        startConnection(false);
      }
      break;

    case State::CONNECTING:
      if (WiFi.status() == WL_CONNECTED) {
        reportConnected();
      } else if (millis() - stateStartedAt_ >= CONNECT_TIMEOUT_MS) {
        scheduleRetry("CONNECT_TIMEOUT");
      }
      break;

    case State::CONNECTED:
      if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[WIFI] Connection lost.");
        bleManager_.sendResponse("WIFI_DISCONNECTED");
        scheduleRetry("CONNECTION_LOST");
      }
      break;

    case State::WAITING_TO_RETRY:
      if (millis() - stateStartedAt_ >= RETRY_INTERVAL_MS) {
        startConnection(true);
      }
      break;

    case State::FAILED:
      // A new firmware session or explicit retry feature will be needed
      // before another connection attempt is started.
      break;
  }
}

bool WiFiManager::isConnected() const {
  return state_ == State::CONNECTED && WiFi.status() == WL_CONNECTED;
}

void WiFiManager::startConnection(bool isRetry) {
  if (!bleManager_.hasWiFiCredentials()) {
    Serial.println("[WIFI] Cannot connect: credentials are not available.");
    state_ = State::WAITING_FOR_CREDENTIALS;
    return;
  }

  if (isRetry) {
    ++retryCount_;
    Serial.print("[WIFI] Reconnection attempt ");
    Serial.print(retryCount_);
    Serial.print("/");
    Serial.println(MAX_RETRIES);
  } else {
    Serial.println("[WIFI] Starting connection...");
    bleManager_.sendResponse("WIFI_CONNECTING");
  }

  const String ssid = bleManager_.getProvisionedSSID();
  const String password = bleManager_.getProvisionedPassword();

  Serial.print("[WIFI] Connecting to SSID: ");
  Serial.println(ssid);

  WiFi.disconnect();
  WiFi.begin(ssid.c_str(), password.c_str());

  state_ = State::CONNECTING;
  stateStartedAt_ = millis();
}

void WiFiManager::scheduleRetry(const String& reason) {
  WiFi.disconnect();

  if (retryCount_ >= MAX_RETRIES) {
    state_ = State::FAILED;
    Serial.print("[WIFI] Connection failed. Reason: ");
    Serial.println(reason);
    bleManager_.sendResponse("WIFI_FAILED|" + reason);
    return;
  }

  state_ = State::WAITING_TO_RETRY;
  stateStartedAt_ = millis();

  Serial.print("[WIFI] ");
  Serial.print(reason);
  Serial.print(". Retrying in ");
  Serial.print(RETRY_INTERVAL_MS / 1000);
  Serial.println(" seconds.");

  bleManager_.sendResponse("WIFI_RETRYING|" +
                           String(retryCount_ + 1) + "/" +
                           String(MAX_RETRIES));
}

void WiFiManager::reportConnected() {
  state_ = State::CONNECTED;
  retryCount_ = 0;

  const String ipAddress = WiFi.localIP().toString();

  Serial.println("[WIFI] Connected successfully.");
  Serial.print("[WIFI] IP address: ");
  Serial.println(ipAddress);
  Serial.print("[WIFI] Signal strength: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  bleManager_.sendResponse("WIFI_CONNECTED|" + ipAddress);
}
