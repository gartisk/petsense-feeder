#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include "config.h"

extern String logBuffer;  // Declared in Logger.cpp or main file

inline const char* msgToCStr(const String& s) { return s.c_str(); }
inline const char* msgToCStr(const char* s) { return s; }

inline String getTimestamp() {
  unsigned long ms = millis();
  unsigned long h = ms / 3600000;
  ms %= 3600000;
  unsigned long m = ms / 60000;
  ms %= 60000;
  unsigned long s = ms / 1000;
  ms %= 1000;
  char buf[16];
  snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu:%03lu", h, m, s, ms);
  return String(buf);
}


inline const char* logLevelName(int level) {
    switch(level) {
        case 1: return "ERROR";
        case 2: return "WARN";
        case 3: return "INFO";
        case 4: return "DEBUG";
        default: return "LOG";
    }
}


inline void appendToLogBuffer(const String& entry) {
  logBuffer += entry;
  if (logBuffer.length() > LOG_BUFFER_MAX_SIZE) {
    // Trim the beginning to keep buffer within limit
    int excess = logBuffer.length() - LOG_BUFFER_MAX_SIZE;
    logBuffer = logBuffer.substring(excess);
  }
}

// [00:00:02:838] [3] [] setup_rfid_module() @ RFIDManager.cpp:30 - MFRC522 ready.
#define LOG_NORMAL(level, msg) \
  if (LOG_LEVEL >= level) { \
    char logLine[256]; \
    snprintf(logLine, sizeof(logLine), "[%s] [%s] - %s\n", \
      getTimestamp().c_str(), logLevelName(level), msgToCStr(msg) ); \
    Serial.print(logLine); \
    appendToLogBuffer(String(logLine)); \
  }


// Base logging macro with buffering
#define LOG_BASE(level, module, func, msg) \
  if (LOG_LEVEL >= level) { \
    char logLine[256]; \
    snprintf(logLine, sizeof(logLine), "[%s] [%s] [%s] %s() @ %s:%d - %s\n", \
      getTimestamp().c_str(), logLevelName(level), module, func, __FILE__, __LINE__, msgToCStr(msg) ); \
    Serial.print(logLine); \
    appendToLogBuffer(String(logLine)); \
  }

#define LOG_ERROR(module, func, msg) LOG_BASE(1, module, func, msg)
#define LOG_WARN(module, func, msg)  LOG_BASE(2, module, func, msg)
#define LOG_INFO(msg) LOG_NORMAL(3, msg)
#define LOG_DEBUG(module, func, msg) LOG_BASE(4, module, func, msg)

#endif  // LOGGER_H
