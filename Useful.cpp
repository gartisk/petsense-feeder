#include "Useful.h"
#include "Log.h"

// Utility function for LED error indication
// Keeping it in the main sketch for simplicity, but could be moved to a 'utils.h/.cpp'

bool blinkingLED ( int ledPin, int blinkingTimes, int ledDelay) {
  for ( int i = 0; i <= blinkingTimes; i++ ){
    digitalWrite(ledPin, HIGH);
    delay(ledDelay);

    digitalWrite(ledPin, LOW);
    delay(ledDelay);
  }

  return false;
}

bool ledStatus ( int blinkingTimes, int ledDelay) {
  return blinkingLED(LED_STATUS, blinkingTimes, ledDelay);
}

bool ledError ( int blinkingTimes, int ledDelay) {
  return blinkingLED(LED_STATUS, blinkingTimes, ledDelay);
}
