#include <Arduino.h>
#include "src/Config/Config.h"
#include "src/Communication/BLEManager.h"

BLEManager bleManager;

void printStartupInformation();

void setup() {
  Serial.begin(MINISKIDSTER_SERIAL_BAUD);
  delay(500);

  printStartupInformation();

  if (!bleManager.begin()) {
    Serial.println("[SYSTEM] BLE startup failed.");
  }
}

void loop() {
  // Stage 1.3: MiniSkidster BLE GATT interface and command RX test only.
  // Wi-Fi, protocol and vehicle-control tasks will be added in later stages.
  delay(1000);
}

void printStartupInformation() {
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("          MINI SKIDSTER"));
  Serial.println(F("========================================"));
  Serial.print(F("Firmware: "));
  Serial.println(MINISKIDSTER_FIRMWARE_VERSION);
  Serial.print(F("Board: "));
  Serial.println(MINISKIDSTER_BOARD_NAME);
  Serial.print(F("Chip: "));
  Serial.println(ESP.getChipModel());
  Serial.print(F("CPU frequency: "));
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(F(" MHz"));
  Serial.println(F("Stage: 1.3 - BLE GATT interface alignment"));
  Serial.println(F("Wi-Fi connection: not implemented"));
  Serial.println(F("Motor/servo outputs: not initialized"));
  Serial.println(F("System startup complete."));
  Serial.println(F("========================================"));
}
