#include "BLEManager.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "../Config/Config.h"

namespace {

bool g_bleConnected = false;

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

}  // namespace

bool BLEManager::begin() {
  Serial.println();
  Serial.println("[BLE] Initializing BLE...");

  BLEDevice::init(MINISKIDSTER_BLE_DEVICE_NAME);

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

  // A read-only identification characteristic confirms that the GATT
  // service is available. Provisioning characteristics come in Stage 1.3.
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
  Serial.println("[BLE] Waiting for phone connection...");

  return true;
}

bool BLEManager::isConnected() const {
  return g_bleConnected;
}
