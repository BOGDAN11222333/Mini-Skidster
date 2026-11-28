#include "WiFiManager.h"

#include <WiFi.h>

namespace {

// Escape a string for safe inclusion as a JSON string value.
String jsonEscape(const String& value) {
  String escaped;
  escaped.reserve(value.length() + 8);

  for (size_t i = 0; i < value.length(); ++i) {
    const char c = value.charAt(i);

    switch (c) {
      case '"': escaped += "\\\""; break;
      case '\\': escaped += "\\\\"; break;
      case '\b': escaped += "\\b"; break;
      case '\f': escaped += "\\f"; break;
      case '\n': escaped += "\\n"; break;
      case '\r': escaped += "\\r"; break;
      case '\t': escaped += "\\t"; break;
      default:
        if (static_cast<uint8_t>(c) < 0x20) {
          const char hex[] = "0123456789ABCDEF";
          escaped += "\\u00";
          escaped += hex[(static_cast<uint8_t>(c) >> 4) & 0x0F];
          escaped += hex[static_cast<uint8_t>(c) & 0x0F];
        } else {
          escaped += c;
        }
        break;
    }
  }

  return escaped;
}

}  // namespace

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

  Serial.println("[WIFI] Ready. Send <WIFI_SCAN> to discover nearby networks.");
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
    return "WIFI_SCAN_FAILED|START";
  }

  state_ = State::SCANNING;
  stateStartedAt_ = millis();

  Serial.println("[WIFI] Asynchronous network scan started.");
  return "WIFI_SCAN_STARTED";
}

String WiFiManager::requestConnection() {
  if (state_ != State::WAITING_FOR_REQUEST && state_ != State::FAILED) {
    return "WIFI_CONNECT_BUSY";
  }

  if (!bleManager_.hasWiFiCredentials()) {
    return "ERROR|WIFI_CREDENTIALS_MISSING";
  }

  connectionRequested_ = true;
  retryCount_ = 0;
  state_ = State::WAITING_FOR_REQUEST;

  Serial.println("[WIFI] Connection requested; credentials accepted.");
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
      // A new explicit WIFI_CONNECT request can start another attempt.
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
    bleManager_.sendResponse("WIFI_SCAN_FAILED");
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

  // Each network is a separate framed BLE message. SSIDs are JSON-escaped,
  // so punctuation and delimiter characters in a network name are safe.
  bleManager_.sendResponse("WIFI_SCAN_BEGIN|" + String(reportedCount));

  for (uint8_t i = 0; i < reportedCount; ++i) {
    const String ssid = WiFi.SSID(i);
    const int32_t rssi = WiFi.RSSI(i);
    const bool secure = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;

    const String payload =
        String("WIFI_NETWORK|{\"ssid\":\"") + jsonEscape(ssid) +
        "\",\"rssi\":" + String(rssi) +
        ",\"secure\":" + (secure ? "true" : "false") + "}";

    Serial.print("[WIFI] Network: ");
    Serial.print(ssid);
    Serial.print(" (");
    Serial.print(rssi);
    Serial.println(" dBm)");

    bleManager_.sendResponse(payload);
  }

  bleManager_.sendResponse("WIFI_SCAN_END");
}

bool WiFiManager::isConnected() const {
  return state_ == State::CONNECTED && WiFi.status() == WL_CONNECTED;
}

void WiFiManager::startConnection(bool isRetry) {
  if (!bleManager_.hasWiFiCredentials()) {
    Serial.println("[WIFI] Cannot connect: credentials are not available.");
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
