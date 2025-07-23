#include <time.h>
#include "config.h"

void timeSetup() {
  // ...existing code...

  // NTP setup
  configTime(NTP_TIMEZONE_UTC * NTP_TIMEZONE_OFFSET, 0, NTP_SERVER_ADDRESS, NTP_SERVER_ADDRESS_ALT); // UTC, use your timezone offset if needed
  Serial.println("Waiting for NTP time sync...");
  while (time(nullptr) < 100000) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("\nNTP time synced.");
}

char* getLocalTime() {
  static char buf[32];
  time_t now = time(nullptr);
  struct tm* timeinfo = localtime(&now);
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeinfo);
  return buf;
}