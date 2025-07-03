#ifndef USEFUL_H
#define USEFUL_H

#include <Arduino.h>
#include "config.h" // Ensure this includes the LED_STATUS definition

bool ledStatus ( int blinkingTimes, int ledDelay);
bool ledError ( int blinkingTimes, int ledDelay);

#endif // USEFUL_H