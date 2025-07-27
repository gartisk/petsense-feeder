#ifndef RFIDMANAGER_H
#define RFIDMANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#include <deque>
#include "config.h"
#include "Useful.h"
#include "Log.h"
#include "SettingsManager.h"

// Define the MFRC522 instance here. It must only be defined once.
// RFID_SPI_PIN and RFID_RST_PIN are defined in config.h
extern MFRC522 mfrc522;

struct RFIDEntry {
    String id;
    unsigned long timestamp;
};

extern std::deque<RFIDEntry> lastRFIDs;

void setup_rfid_module();
void addRFIDToHistory(const RFIDEntry& entry);
String convertUidToString(const byte* uid, byte size);
String scan_rfid_card();
bool isAllowedRFID(const String& rfid);

#endif // RFIDMANAGER_H