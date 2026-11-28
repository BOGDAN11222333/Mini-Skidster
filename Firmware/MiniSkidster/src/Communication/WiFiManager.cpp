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

  state_ = State::WAITING_FOR_REQUEST;
  stateStartedAt_ = millis();
  retryCount_ = 0;
  connectionRequested_ = false;

  Serial.println("[WIFI] Ready. App can request a scan.");
}

String WiFiManager::requestScan() {
  if (state_ != State::WAITING_FOR_REQUEST && state_ != State::FAILED) {
    return "WIFI_SCAN_BUSY";
  }

  if (WiFi.status() == WL_CONNECTED) {
    return "WIFI_SCAN_UNAVAILABLE|ALREADY_CONNECTED";
  }

  WiFi.scanDelete();
  const int16_t result = WiFi.scanNetworks(true, false);

  if (result == WIFI_SCAN_FAILED) {
    Serial.println("[WIFI] Could not start asynchronous scan.");
    return "WIFI_SCAN_FAILED";
  }

  state_ = State::SCANNING;
  stateStartedAt_ = millis();

  Serial.println("[WIFI] Asynchronous network scan started.");
  return "WIFI_SCAN_STARTED";
}

String WiFiManager::requestConnection(const String& ssid,
                                      const String& password) {
  if (state_ != State::WAITING_FOR_REQUEST && state_ != State::FAILED) {
    return "WIFI_CONNECT_BUSY";
  }

  if (ssid.isEmpty() || ssid.length() > 32 || password.length() > 63) {
    return "ERROR|INVALID_WIFI_CREDENTIALS";
  }

  ssid_ = ssid;
  password_ = password;
  connectionRequested_ = true;
  retryCount_ = 0;
  state_ = State::WAITING_FOR_REQUEST;

  Serial.print("[WIFI] Credentials accepted for SSID: ");
  Serial.println(ssid_);
  return "WIFI_CONNECT_REQUESTED";
}

void WiFiManager::update() {
  switch (state_) {
    case State::WAITING_FOR_REQUEST:
      if (connectionRequested_) {
        connectionRequested_ = false;
        startConnection(false);
      }
      break;

    case State::SCANNING:
      updateScan();
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
        bleManager_.sendResponse("<WIFI_DISCONNECTED>");
        scheduleRetry("CONNECTION_LOST");
      }
      break;

    case State::WAITING_TO_RETRY:
      if (millis() - stateStartedAt_ >= RETRY_INTERVAL_MS) {
        startConnection(true);
      }
      break;

    case State::FAILED:
      // A new explicit WIFI_CONNECT command can start another attempt.
      break;
  }
}

void WiFiManager::updateScan() {
  const int16_t result = WiFi.scanComplete();

  if (result == WIFI_SCAN_RUNNING) {
    return;
  }

  if (result == WIFI_SCAN_FAILED) {
    Serial.println("[WIFI] Network scan failed.");
    bleManager_.sendResponse("<WIFI_LIST_START>");
    bleManager_.sendResponse("<WIFI_LIST_END>");
    WiFi.scanDelete();
    state_ = State::WAITING_FOR_REQUEST;
    return;
  }

  reportScanResults(result);
  WiFi.scanDelete();
  state_ = State::WAITING_FOR_REQUEST;
}

void WiFiManager::reportScanResults(int16_t networkCount) {
  const uint8_t reportedCount =
      networkCount > MAX_REPORTED_NETWORKS
          ? MAX_REPORTED_NETWORKS
          : static_cast<uint8_t>(networkCount);

  Serial.print("[WIFI] Scan complete. Networks found: ");
  Serial.println(networkCount);

  // These message names and payloads match the existing MIT App Inventor
  // receive parser: clear list, append each WIFI value, then finish the list.
  bleManager_.sendResponse("<WIFI_LIST_START>");

  for (uint8_t i = 0; i < reportedCount; ++i) {
    const String ssid = WiFi.SSID(i);
    if (ssid.isEmpty()) {
      continue;
    }

    Serial.print("[WIFI] Network found: ");
    Serial.print(ssid);
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.println(" dBm)");

    bleManager_.sendResponse("<WIFI:" + ssid + "," + String(WiFi.RSSI(i)) + ">");
  }

  bleManager_.sendResponse("<WIFI_LIST_END>");
}

bool WiFiManager::isConnected() const {
  return state_ == State::CONNECTED && WiFi.status() == WL_CONNECTED;
}

void WiFiManager::startConnection(bool isRetry) {
  if (ssid_.isEmpty()) {
    Serial.println("[WIFI] Cannot connect: SSID is empty.");
    state_ = State::WAITING_FOR_REQUEST;
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
    bleManager_.sendResponse("<WIFI_CONNECTING>");
  }

  Serial.print("[WIFI] Connecting to SSID: ");
  Serial.println(ssid_);

  WiFi.disconnect();
  WiFi.begin(ssid_.c_str(), password_.c_str());

  state_ = State::CONNECTING;
  stateStartedAt_ = millis();
}

void WiFiManager::scheduleRetry(const String& reason) {
  WiFi.disconnect();

  if (retryCount_ >= MAX_RETRIES) {
    state_ = State::FAILED;
    Serial.print("[WIFI] Connection failed. Reason: ");
    Serial.println(reason);
    bleManager_.sendResponse("<WIFI_FAILED>");
    return;
  }

  state_ = State::WAITING_TO_RETRY;
  stateStartedAt_ = millis();

  Serial.print("[WIFI] ");
  Serial.print(reason);
  Serial.print(". Retrying in ");
  Serial.print(RETRY_INTERVAL_MS / 1000);
  Serial.println(" seconds.");

  bleManager_.sendResponse("<WIFI_RETRYING>");
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

  bleManager_.sendResponse("<WIFI_CONNECTED:" + ipAddress + ">");
}
