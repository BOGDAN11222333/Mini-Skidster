#pragma once

#include <Arduino.h>

// -----------------------------------------------------------------------------
// MiniSkidster firmware configuration
// Board: ESP32-C3 Super Mini
// Development stage: 1.1 - Firmware foundation
// -----------------------------------------------------------------------------

#define MINISKIDSTER_FIRMWARE_VERSION "1.0.0-dev.1"
#define MINISKIDSTER_BOARD_NAME "ESP32-C3 Super Mini"
#define MINISKIDSTER_SERIAL_BAUD 115200

namespace MiniSkidsterConfig {

// DRV8833 motor driver pin assignments.
// These assignments are provisional until the wiring is physically verified.
constexpr uint8_t MOTOR_LEFT_IN1_PIN  = 3;
constexpr uint8_t MOTOR_LEFT_IN2_PIN  = 4;
constexpr uint8_t MOTOR_RIGHT_IN1_PIN = 5;
constexpr uint8_t MOTOR_RIGHT_IN2_PIN = 6;

// Servo signal pin assignments.
// Servo outputs are not configured or activated in Stage 1.1.
constexpr uint8_t ARM_SERVO_PIN    = 7;
constexpr uint8_t BUCKET_SERVO_PIN = 10;

}  // namespace MiniSkidsterConfig
