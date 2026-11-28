
#include "CommandProcessor.h"

String CommandProcessor::processCommand(const String& command) const {
  String payload = command;
  payload.trim();

  // Remove command framing: < and >
  if (payload.startsWith("<")) {
    payload.remove(0, 1);
  }

  if (payload.endsWith(">")) {
    payload.remove(payload.length() - 1);
  }

  payload.trim();

  // PING command
  if (payload == "PING") {
    return "PONG";
  }

  // ECHO command
  if (payload.startsWith("ECHO|")) {
    return payload;
  }

  // Unknown command
  return "ERROR|UNKNOWN_COMMAND";
}
