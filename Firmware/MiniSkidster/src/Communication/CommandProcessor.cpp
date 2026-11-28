#include "CommandProcessor.h"

String CommandProcessor::processCommand(const String& command) const {
  ParsedCommand parsed;
  const ProtocolResult result = protocol_.parseCommand(command, parsed);

  switch (result) {
    case ProtocolResult::UNKNOWN_COMMAND:
      return "ERROR|UNKNOWN_COMMAND";

    case ProtocolResult::INVALID_FORMAT:
      return "ERROR|INVALID_FORMAT";

    case ProtocolResult::INVALID_PARAMETER:
      return "ERROR|INVALID_PARAMETER";

    case ProtocolResult::SUCCESS:
      break;
  }

  switch (parsed.type) {
    case CommandType::PING:
      return "PONG";

    case CommandType::ECHO:
      return "ECHO|" + parsed.text;

    case CommandType::DRIVE:
      return "PARSED|DRIVE|" + String(parsed.value1) + "|" + String(parsed.value2);

    case CommandType::ARM:
      return "PARSED|ARM|" + String(parsed.value1);

    case CommandType::BUCKET:
      return "PARSED|BUCKET|" + String(parsed.value1);

    case CommandType::STOP:
      return "PARSED|STOP";

    case CommandType::STATUS:
      return "PARSED|STATUS";
  }

  return "ERROR|UNKNOWN_COMMAND";
}
