#include <Arduino.h>
#include "Config/Config.h"

void printStartupInformation();

void setup() {
  Serial.begin(MINISKIDSTER_SERIAL_BAUD);
  delay(500);

  printStartupInformation();
}

void loop() {
  // Stage 1.1 intentionally has no vehicle-control or communication tasks.
  // BLE, Wi-Fi and hardware modules will be added in later stages.
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
  Serial.println(F("Stage: 1.1 - Firmware foundation"));
  Serial.println(F("BLE: not implemented"));
  Serial.println(F("Wi-Fi: not implemented"));
  Serial.println(F("Motor/servo outputs: not initialized"));
  Serial.println(F("System startup complete."));
  Serial.println(F("========================================"));
}
