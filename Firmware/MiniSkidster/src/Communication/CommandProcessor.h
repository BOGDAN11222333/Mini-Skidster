#pragma once

#include <Arduino.h>

#include "Protocol.h"

// Interprets complete commands received by the communication layer.
// This stage validates and describes commands; it does not control hardware.
class CommandProcessor {
public:
  String processCommand(const String& command) const;

private:
  Protocol protocol_;
};
