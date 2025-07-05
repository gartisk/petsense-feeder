#include "config.h" 
#include "SettingsManager.h"
#include "Log.h"
#include "Useful.h"

#include "Webserver.h" // Handles all web server functionalities
#include "RFIDManager.h"
#include "DoorController.h"

String lastScannedRfidUID = "No scan yet"; // Stores the last scanned RFID UID for web display


void setup() {
  Serial.begin(SERIAL_BEGIN); // Initialize serial communication
  LOG_INFO("Pet Feeder - Setup Started");

  // Initialize LittleFS filesystem
  LOG_INFO("Settings Manager - Initializing");
  SettingsManager::begin(); 

  // Initialize LEDs
  LOG_INFO("LEDs - Initializing");
  pinMode(LED_STATUS, OUTPUT);
  pinMode(LED_ERROR, OUTPUT);

  // Initialize the LED status
  LED_MSG_START();

  // Initialize the RFID module
  LOG_INFO("RFID Module - Initializing");
  setup_rfid_module();

  // Initialize Wi-Fi and setup the web server
  LOG_INFO("Web Server - Initializing");
  setup_webserver();

  // Initialize the door controller
  LOG_INFO("Door Controller - Initializing");
  DoorController::begin();
}

void scan_id(){
  // Continuously scan for RFID tags
  String rfidUid = scan_rfid_card(); // Scan for RFID card and get UID

  // nothing scanned
  if ( rfidUid.length() < 1 ) {
    return;
  }
  ledStatus(1, 100); // Indicate scanning with LED

  if ( !isAllowedRFID(rfidUid) ) { 
    
    LED_ERROR_RFID_INVALID();
    LOG_ERROR("RFIDManager", __FUNCTION__, "RFID not allowed: " + rfidUid);
    
    lastScannedRfidUID = "Not allowed: " + rfidUid; // Update last scanned RFID UID
    return; // Exit if RFID is not allowed
  }

  ledStatus(2, 100); // Indicate success with LED
  LOG_INFO("RFID allowed: " + rfidUid);
  lastScannedRfidUID = "Allowed: " + rfidUid; // Update last scanned RFID UID
  
  DoorController::open_close();                // If a valid RFID UID is scanned, open the door
}

void loop() {
  scan_id(); // Call the function to scan for RFID tags

  // Handle incoming HTTP requests for the web server
  server.handleClient(); // Process incoming HTTP requests for the web server

  DoorController::toggle();
  delay(500);
}
