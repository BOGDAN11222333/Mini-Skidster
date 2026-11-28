#pragma once

#include <Arduino.h>

// Parsed command types. These describe requests only; they do not activate hardware.
enum class CommandType {
  PING,
  ECHO,
  DRIVE,
  ARM,
  BUCKET,
  STOP,
  STATUS
};

struct ParsedCommand {
  CommandType type = CommandType::PING;
  int value1 = 0;
  int value2 = 0;
  String text;
};

enum class ProtocolResult {
  SUCCESS,
  UNKNOWN_COMMAND,
  INVALID_FORMAT,
  INVALID_PARAMETER
};

// Validates a complete angle-bracket command and extracts its parameters.
class Protocol {
public:
  ProtocolResult parseCommand(const String& framedCommand, ParsedCommand& parsed) const;

private:
  bool parseInteger(const String& value, int& result) const;
};
