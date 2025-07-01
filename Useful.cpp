#include "Useful.h"
#include "Log.h"

// Utility function for LED error indication
// Keeping it in the main sketch for simplicity, but could be moved to a 'utils.h/.cpp'
bool ledStatus ( int blinkingTimes, int ledDelay) {
  pinMode(LED_STATUS, OUTPUT); // Ensure the onboard LED pin is set as output
  for ( int i = 0; i <= blinkingTimes; i++ ){
    digitalWrite(LED_STATUS, HIGH);
    delay(ledDelay);

    digitalWrite(LED_STATUS, LOW);
    delay(ledDelay);
  }

  return false;
}

// void logMessage(const char* message) {
//     Serial.println(message);
// }

// void logMessage(const String& message) {
//     Serial.println(message.c_str());
// }

// void logError(const char* message) {
//     Serial.print("ERROR: ");
//     Serial.println(message);
// }

// void logError(const String& message) {
//     Serial.print("ERROR: ");
//     Serial.println(message.c_str());
// }