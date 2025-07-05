#include "config.h"
#include <Arduino.h>


// Non-blocking LED blink state
unsigned long ledBlinkStart = 0;
int ledBlinkTimes = 0;
int ledBlinkCount = 0;
int ledBlinkDelay = 0;
int ledBlinkPin = LED_STATUS;
bool ledBlinkActive = false;
bool ledBlinkState = false;

void startLedBlink(int pin, int times, int delayMs) {
  ledBlinkPin = pin;
  ledBlinkTimes = times;
  ledBlinkDelay = delayMs;
  ledBlinkCount = 0;
  ledBlinkActive = true;
  ledBlinkStart = millis();
  ledBlinkState = false;
  digitalWrite(ledBlinkPin, LOW);
}

void updateLedBlink() {
  if (!ledBlinkActive) return;
  if (millis() - ledBlinkStart >= ledBlinkDelay) {
    ledBlinkStart = millis();
    ledBlinkState = !ledBlinkState;
    digitalWrite(ledBlinkPin, ledBlinkState ? HIGH : LOW);
    if (!ledBlinkState) { // Only count completed on-off cycles
      ledBlinkCount++;
      if (ledBlinkCount >= ledBlinkTimes) {
        ledBlinkActive = false;
        digitalWrite(ledBlinkPin, LOW);
      }
    }
  }
}