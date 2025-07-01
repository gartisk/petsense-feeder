
#ifndef RFIDMANAGER_H
#define RFIDMANAGER_H

#include <SPI.h>
#include <MFRC522.h>

#include <deque>
#include "config.h"
#include "Useful.h"
#include "Log.h"

struct RFIDEntry {
    String id;
    unsigned long timestamp;
};

extern std::deque<RFIDEntry> lastRFIDs;

void setup_rfid_module();
String convertUidToString(const byte* uid, byte size);
String scan_rfid_card();

#endif // RFIDMANAGER_H