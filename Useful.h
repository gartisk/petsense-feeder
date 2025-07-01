#ifndef USEFUL_H
#define USEFUL_H

#include <Arduino.h>

#include "config.h" // Ensure this includes the LED_STATUS definition

bool ledStatus ( int errorId, int ledDelay);

// @TODO: erase
// void logMessage (const char* message);
// void logMessage(const String& message);

// void logError(const char* message);
// void logError(const String& message);

#endif // USEFUL_H