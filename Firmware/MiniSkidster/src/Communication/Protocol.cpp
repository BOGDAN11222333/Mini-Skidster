#include "Protocol.h"

ProtocolResult Protocol::parseCommand(const String& framedCommand, ParsedCommand& parsed) const {
  String payload = framedCommand;
  payload.trim();

  // Commands must arrive as one complete <...> frame.
  if (payload.length() < 3 || !payload.startsWith("<") || !payload.endsWith(">")) {
    return ProtocolResult::INVALID_FORMAT;
  }

  payload = payload.substring(1, payload.length() - 1);
  payload.trim();

  if (payload == "PING") {
    parsed.type = CommandType::PING;
    return ProtocolResult::SUCCESS;
  }

  if (payload == "STOP") {
    parsed.type = CommandType::STOP;
    return ProtocolResult::SUCCESS;
  }

  if (payload == "STATUS") {
    parsed.type = CommandType::STATUS;
    return ProtocolResult::SUCCESS;
  }

  if (payload.startsWith("ECHO|")) {
    parsed.type = CommandType::ECHO;
    parsed.text = payload.substring(5);
    return ProtocolResult::SUCCESS;
  }

  if (payload.startsWith("DRIVE|")) {
    const int separator = payload.indexOf('|', 6);
    if (separator < 0 || payload.indexOf('|', separator + 1) >= 0) {
      return ProtocolResult::INVALID_FORMAT;
    }

    String leftValue = payload.substring(6, separator);
    String rightValue = payload.substring(separator + 1);
    leftValue.trim();
    rightValue.trim();

    if (!parseInteger(leftValue, parsed.value1) ||
        !parseInteger(rightValue, parsed.value2)) {
      return ProtocolResult::INVALID_FORMAT;
    }

    if (parsed.value1 < -100 || parsed.value1 > 100 ||
        parsed.value2 < -100 || parsed.value2 > 100) {
      return ProtocolResult::INVALID_PARAMETER;
    }

    parsed.type = CommandType::DRIVE;
    return ProtocolResult::SUCCESS;
  }

  if (payload.startsWith("ARM|") || payload.startsWith("BUCKET|")) {
    const bool isArm = payload.startsWith("ARM|");
    String angleValue = payload.substring(isArm ? 4 : 7);
    angleValue.trim();

    if (!parseInteger(angleValue, parsed.value1)) {
      return ProtocolResult::INVALID_FORMAT;
    }

    if (parsed.value1 < 0 || parsed.value1 > 180) {
      return ProtocolResult::INVALID_PARAMETER;
    }

    parsed.type = isArm ? CommandType::ARM : CommandType::BUCKET;
    return ProtocolResult::SUCCESS;
  }

  return ProtocolResult::UNKNOWN_COMMAND;
}

bool Protocol::parseInteger(const String& value, int& result) const {
  if (value.isEmpty()) {
    return false;
  }

  size_t index = 0;
  if (value[0] == '-') {
    if (value.length() == 1) {
      return false;
    }
    index = 1;
  }

  for (; index < value.length(); ++index) {
    if (value[index] < '0' || value[index] > '9') {
      return false;
    }
  }

  result = value.toInt();
  return true;
}
