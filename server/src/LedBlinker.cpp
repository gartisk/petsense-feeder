#include "LedBlinker.h"

// You must provide the pin numbers here or call begin(pin) in setup()
LedBlinker GreenBlinker::staticInstance(LED_BUILTIN); // or your green LED pin
LedBlinker RedBlinker::staticInstance(LED_BUILTIN);   // or your red LED pin