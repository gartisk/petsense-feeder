
#include <SPI.h>
#include <MFRC522.h>
#include "config.h"
#include "RFIDManager.h"
// Define the MFRC522 instance here. It must only be defined once.
// RFID_SPI_PIN and RFID_RST_PIN are defined in config.h
MFRC522 mfrc522(RFID_SPI_PIN, RFID_RST_PIN);

const size_t MAX_RFID_HISTORY = RFID_HISTORY_SIZE;

std::deque<RFIDEntry> lastRFIDs;

void addRFIDToHistory(const RFIDEntry& entry) {
    // Check for duplicates
    for (const auto& e : lastRFIDs) {
        if (e.id == entry.id) {
            return; // Already present, do not add
        }
    }
    // Add new entry
    lastRFIDs.push_back(entry);
    
    // Maintain max history size
    if (lastRFIDs.size() > MAX_RFID_HISTORY) {
        lastRFIDs.pop_front();
    }
}

void setup_rfid_module() {
    // Initialize SPI bus
    SPI.begin();       
    delay(RFID_SPI_DELAY);  

    // Initialize MFRC522 PCD
    mfrc522.PCD_Init();
    delay(RFID_DELAY);

    LOG_INFO("RFID Init done. Checking version...");
    
    byte version = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
    char message[30];
    snprintf(message, sizeof(message), "MFRC522 Version: 0x%02X", version);
    LOG_INFO(message);

    if (version == 0x00 || version == 0xFF) {
        LOG_ERROR( "setup_rfid_module", __FUNCTION__, "MFRC522 not found! Check wiring and power.");
        ledError(2, 100); // Indicate error with LED
        return; // Halt setup if MFRC522 is not found
    } 

    LOG_INFO("MFRC522 ready.");
}


// 
String convertUidToString(const byte* uid, byte size) {
    String sCardID = "";
    for (byte i = 0; i < size; i++) {
        if (i > 0) sCardID += ":";
        sCardID += uid[i] < 0x10 ? "0" : "";
        sCardID += String(uid[i], HEX);
    }
    sCardID.toUpperCase();

    return sCardID;
}

// Scan RFID Card return the UID as a char array
// Call anonymous function passed to the function
String scan_rfid_card() {
    // Look for new cards
    if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
        // No card or failed to read
        return "";
    }

    ledStatus(2, 500); // Reading card, indicate with LED

    // Read the card UID
    String sCardID = convertUidToString(mfrc522.uid.uidByte, mfrc522.uid.size);
    // Add to history
    addRFIDToHistory({sCardID, millis()});

    LOG_INFO("Card scanned: " + sCardID);

    // Halt PICC
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    return sCardID;
}

bool isAllowedRFID(const String& rfid) {
    // Check if the RFID is in the allowed list
    JsonObject settings;
    if ( !SettingsManager::cached_settings.containsKey("RFID_LIST") ) {
        LOG_ERROR("RFIDManager", __FUNCTION__, "Failed to get settings.");
        return false;
    }

    if (!settings.containsKey("RFID_LIST")) {
        LOG_ERROR("RFIDManager", __FUNCTION__, "RFID_LIST not found in settings.");
        return false;
    }

    JsonArray allowedRFIDs = settings["RFID_LIST"].as<JsonArray>();
    for (const auto& allowedRFID : allowedRFIDs) {
        if (allowedRFID["ID"].as<String>() == rfid && allowedRFID["ALLOWED"].as<bool>()) {
            return true; // RFID is allowed
        }
    }

    LOG_ERROR("RFIDManager", __FUNCTION__, "RFID not found in allowed list.");
    return false; // RFID not found in allowed list
}