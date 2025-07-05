#include "config.h"
#include <Arduino.h>

// LED MESSAGES DEFINITION

void BLINK_LED ( int ledPin, int blinkingTimes, int ledDelay) {
  int i = 0; // Initialize counter
  for ( ; i <= blinkingTimes; i++ ){
    digitalWrite(ledPin, HIGH);
    delay(ledDelay);

    digitalWrite(ledPin, LOW);
    delay(ledDelay);
  }
}