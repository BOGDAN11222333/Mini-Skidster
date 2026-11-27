#include "BLEManager.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <string>

#include "../Config/Config.h"

namespace {

bool g_bleConnected = false;
String g_wifiSSID;
String g_wifiPassword;
bool g_ssidReceived = false;
bool g_passwordReceived = false;

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
    const std::string value = characteristic->getValue();

    if (isSSID_) {
      if (value.empty() || value.length() > 32) {
        g_ssidReceived = false;
        Serial.println("[BLE] Rejected SSID: length must be 1-32 bytes.");
        return;
      }

      g_wifiSSID = String(value.c_str());
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

      g_wifiPassword = String(value.c_str());
      g_passwordReceived = true;
      Serial.print("[BLE] Wi-Fi password received (");
      Serial.print(value.length());
      Serial.println(" bytes).");
    }

    if (g_ssidReceived && g_passwordReceived) {
      Serial.println("[BLE] Wi-Fi credentials received and stored in RAM.");
      Serial.println("[BLE] Wi-Fi connection will be handled in Stage 1.4.");
    }
  }

private:
  bool isSSID_;
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

  BLEService* service =
      server->createService(MINISKIDSTER_BLE_SERVICE_UUID);

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
    Serial.println("[BLE] ERROR: Could not create Wi-Fi provisioning characteristics.");
    return false;
  }

  ssidCharacteristic->setCallbacks(new WiFiCredentialCallbacks(true));
  passwordCharacteristic->setCallbacks(new WiFiCredentialCallbacks(false));

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
  Serial.println("[BLE] Advertising: active");
  Serial.println("[BLE] Wi-Fi credential characteristics: ready");
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
