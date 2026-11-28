#include <Arduino.h>
#include "src/Config/Config.h"
#include "src/Communication/BLEManager.h"
#include "src/Communication/WiFiManager.h"

BLEManager bleManager;
WiFiManager wifiManager(bleManager);

void printStartupInformation();

void setup() {
  Serial.begin(MINISKIDSTER_SERIAL_BAUD);
  delay(500);

  printStartupInformation();

  if (!bleManager.begin()) {
    Serial.println("[SYSTEM] BLE startup failed.");
  }

  wifiManager.begin();
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();

    if (command == 'T' || command == 't') {
      bleManager.sendTestNotification();
    }
  }

  wifiManager.update();

  delay(10);
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
  Serial.println(F("Stage: 2.0 - Wi-Fi connection"));
  Serial.println(F("Wi-Fi: BLE-provisioned station connection"));
  Serial.println(F("HTTP server: not implemented"));
  Serial.println(F("Motor/servo outputs: not initialized"));
  Serial.println(F("Command parsing: validation only; no hardware movement"));
  Serial.println(F("System startup complete."));
  Serial.println(F("========================================"));
}
