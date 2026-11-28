#include "BLEManager.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "../Config/Config.h"

namespace {

bool g_bleConnected = false;
String g_wifiSSID;
String g_wifiPassword;
bool g_ssidReceived = false;
bool g_passwordReceived = false;

BLECharacteristic* g_txCharacteristic = nullptr;
String g_rxBuffer;

class MiniSkidsterServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* server) override {
    g_bleConnected = true;
    Serial.println("[BLE] Phone connected.");
  }

  void onDisconnect(BLEServer* server) override {
    g_bleConnected = false;
    Serial.println("[BLE] Phone disconnected.");
    Serial.println("[BLE] Restarting advertising...");
    BLEDevice::startAdvertising();
  }
};

class WiFiCredentialCallbacks : public BLECharacteristicCallbacks {
public:
  explicit WiFiCredentialCallbacks(bool isSSID) : isSSID_(isSSID) {}

  void onWrite(BLECharacteristic* characteristic) override {
    const String value = characteristic->getValue();

    if (isSSID_) {
      if (value.isEmpty() || value.length() > 32) {
        g_ssidReceived = false;
        Serial.println("[BLE] Rejected SSID: length must be 1-32 bytes.");
        return;
      }

      g_wifiSSID = value;
      g_ssidReceived = true;
      Serial.print("[BLE] Wi-Fi SSID received (");
      Serial.print(value.length());
      Serial.println(" bytes).");
    } else {
      if (value.length() > 63) {
        g_passwordReceived = false;
        Serial.println("[BLE] Rejected Wi-Fi password: maximum 63 bytes.");
        return;
      }

      g_wifiPassword = value;
      g_passwordReceived = true;
      Serial.print("[BLE] Wi-Fi password received (");
      Serial.print(value.length());
      Serial.println(" bytes).");
    }

    if (g_ssidReceived && g_passwordReceived) {
      Serial.println("[BLE] Wi-Fi credentials received and stored in RAM.");
    }
  }

private:
  bool isSSID_;
};

// Stage 1.3 GATT smoke test: receive complete angle-bracket commands.
// Chunk assembly is intentionally kept minimal here; the full protocol
// parser will be implemented as a separate module.
class BLECommandCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* characteristic) override {
    const String chunk = characteristic->getValue();
    if (chunk.isEmpty()) {
      return;
    }

    g_rxBuffer += chunk;

    if (g_rxBuffer.length() > 512) {
      Serial.println("[BLE] RX buffer overflow; clearing incomplete command.");
      g_rxBuffer = "";
      return;
    }

    int start = g_rxBuffer.indexOf('<');
    int end = g_rxBuffer.indexOf('>', start + 1);

    while (start >= 0 && end > start) {
      String command = g_rxBuffer.substring(start, end + 1);
      Serial.print("[BLE] Command received: ");
      Serial.println(command);
      g_rxBuffer.remove(0, end + 1);
      start = g_rxBuffer.indexOf('<');
      end = g_rxBuffer.indexOf('>', start + 1);
    }

    // Discard noise before a possible start marker.
    if (start > 0) {
      g_rxBuffer.remove(0, start);
    }
  }
};

}  // namespace

bool BLEManager::begin() {
  Serial.println();
  Serial.println("[BLE] Initializing BLE...");

  BLEDevice::init(MINISKIDSTER_BLE_DEVICE_NAME);
  BLEDevice::setMTU(100);

  BLEServer* server = BLEDevice::createServer();
  if (server == nullptr) {
    Serial.println("[BLE] ERROR: Could not create BLE server.");
    return false;
  }

  server->setCallbacks(new MiniSkidsterServerCallbacks());

  BLEService* service = server->createService(MINISKIDSTER_BLE_SERVICE_UUID);
  if (service == nullptr) {
    Serial.println("[BLE] ERROR: Could not create BLE service.");
    return false;
  }

  BLECharacteristic* infoCharacteristic = service->createCharacteristic(
      MINISKIDSTER_BLE_INFO_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ);
  if (infoCharacteristic == nullptr) {
    Serial.println("[BLE] ERROR: Could not create info characteristic.");
    return false;
  }

  const String deviceInfo =
      String("MiniSkidster | Firmware ") + MINISKIDSTER_FIRMWARE_VERSION;
  infoCharacteristic->setValue(deviceInfo.c_str());

  BLECharacteristic* ssidCharacteristic = service->createCharacteristic(
      MINISKIDSTER_BLE_WIFI_SSID_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE);
  BLECharacteristic* passwordCharacteristic = service->createCharacteristic(
      MINISKIDSTER_BLE_WIFI_PASSWORD_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE);

  if (ssidCharacteristic == nullptr || passwordCharacteristic == nullptr) {
    Serial.println("[BLE] ERROR: Could not create Wi-Fi credential characteristics.");
    return false;
  }

  ssidCharacteristic->setCallbacks(new WiFiCredentialCallbacks(true));
  passwordCharacteristic->setCallbacks(new WiFiCredentialCallbacks(false));

  BLECharacteristic* rxCharacteristic = service->createCharacteristic(
      MINISKIDSTER_BLE_RX_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  if (rxCharacteristic == nullptr) {
    Serial.println("[BLE] ERROR: Could not create RX characteristic.");
    return false;
  }
  rxCharacteristic->setCallbacks(new BLECommandCallbacks());

  g_txCharacteristic = service->createCharacteristic(
      MINISKIDSTER_BLE_TX_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
  if (g_txCharacteristic == nullptr) {
    Serial.println("[BLE] ERROR: Could not create TX characteristic.");
    return false;
  }
  g_txCharacteristic->setValue("MiniSkidster BLE ready");

  service->start();

  BLEAdvertising* advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(MINISKIDSTER_BLE_SERVICE_UUID);
  advertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  Serial.println("[BLE] Initialization complete.");
  Serial.print("[BLE] Device name: ");
  Serial.println(MINISKIDSTER_BLE_DEVICE_NAME);
  Serial.print("[BLE] Service UUID: ");
  Serial.println(MINISKIDSTER_BLE_SERVICE_UUID);
  Serial.println("[BLE] GATT characteristics:");
  Serial.println("[BLE]   0002 - Device information (READ)");
  Serial.println("[BLE]   0003 - Wi-Fi SSID (WRITE)");
  Serial.println("[BLE]   0004 - Wi-Fi password (WRITE)");
  Serial.println("[BLE]   0005 - Command RX (WRITE)");
  Serial.println("[BLE]   0006 - Response TX (READ/NOTIFY)");
  Serial.println("[BLE] Advertising: active");
  Serial.println("[BLE] Waiting for phone connection...");

  return true;
}

bool BLEManager::isConnected() const {
  return g_bleConnected;
}

bool BLEManager::hasWiFiCredentials() const {
  return g_ssidReceived && g_passwordReceived;
}

String BLEManager::getProvisionedSSID() const {
  return g_wifiSSID;
}

String BLEManager::getProvisionedPassword() const {
  return g_wifiPassword;
}


void BLEManager::sendTestNotification() {
  if (!g_bleConnected || g_txCharacteristic == nullptr) {
    Serial.println("[BLE] Cannot send test notification: phone not connected.");
    return;
  }

  const String message = "@BTEST|ESP32_RESPONSE@E";
  const size_t chunkSize = 20;

  Serial.println("[BLE] Sending test message in chunks:");

  for (size_t offset = 0; offset < message.length(); offset += chunkSize) {
    String chunk = message.substring(offset, offset + chunkSize);

    g_txCharacteristic->setValue(chunk.c_str());
    g_txCharacteristic->notify();

    Serial.print("[BLE] TX chunk: ");
    Serial.println(chunk);

    delay(100);
  }

  Serial.println("[BLE] Test message transmission complete.");
}

