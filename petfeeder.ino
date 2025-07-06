#include "config.h" 
#include "SettingsManager.h"
#include "Log.h"
#include "Useful.h"

#include "Webserver.h" // Handles all web server functionalities
#include "RFIDManager.h"
#include "DoorController.h"
#include "LedBlinker.h" // Handles LED blinking functionalities

String lastScannedRfidUID = "No scan yet"; // Stores the last scanned RFID UID for web display


void setup() {
  Serial.begin(SERIAL_BEGIN); // Initialize serial communication
  LOG_INFO("Pet Feeder - Setup Started");

  // Initialize LittleFS filesystem
  LOG_INFO("Settings Manager - Initializing");
  SettingsManager::begin(); 

  // Initialize LEDs
  LOG_INFO("LEDs - Initializing");
  GreenBlinker::begin(LED_STATUS); // or your green LED pin
  RedBlinker::begin(LED_ERROR);    // or your red LED pin

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

  LED_MSG_RFID_READ(); // Reading card, indicate with LED
  //ledStatus(1, 100); // Indicate scanning with LED

  if ( !isAllowedRFID(rfidUid) ) { 
    LED_ERROR_RFID_INVALID();
    LOG_ERROR("RFIDManager", __FUNCTION__, "RFID not allowed: " + rfidUid);
    
    lastScannedRfidUID = "Not allowed: " + rfidUid; // Update last scanned RFID UID

    return; // Exit if RFID is not allowed
  }

  // Indicate allowed RFID with LED
  LED_MSG_RFID_ALLOWED(); 

  LOG_INFO("RFID allowed: " + rfidUid);
  lastScannedRfidUID = "Allowed: " + rfidUid; // Update last scanned RFID UID
  
  DoorController::open_close();                // If a valid RFID UID is scanned, open the door
}

void loop() {
  scan_id(); // Call the function to scan for RFID tags
  
  // Handle incoming HTTP requests for the web server
  server.handleClient(); // Process incoming HTTP requests for the web server
  
  GreenBlinker::update();
  RedBlinker::update();

  DoorController::process();
  DoorController::toggle();
  
  //updateLedBlink(); 
}
