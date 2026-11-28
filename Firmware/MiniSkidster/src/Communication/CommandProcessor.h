
#pragma once

#include <Arduino.h>

// Interprets complete commands received by the communication layer.
class CommandProcessor {
public:
  String processCommand(const String& command) const;
};