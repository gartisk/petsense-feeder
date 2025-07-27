#include "config.h" 
#include "SettingsManager.h"
#include "Log.h"
#include "Useful.h"
#include "LocalTime.h" // Handles local time functionalities

#include "Webserver.h" // Handles all web server functionalities
#include "RFIDManager.h"
#include "DoorController.h"
#include "LedBlinker.h" // Handles LED blinking functionalities


void setup() {
  Serial.begin(SERIAL_BEGIN); // Initialize serial communication
  LOG_INFO("Pet Feeder - Setup");

  // Initialize LittleFS filesystem
  LOG_INFO("Initializing Settings Manager...");
  SettingsManager::begin(); 
  SettingsManager::load(); 

  // Initialize LEDs
  LOG_INFO("Initializing LEDs...");
  GreenBlinker::begin(LED_STATUS); // or your green LED pin
  RedBlinker::begin(LED_ERROR);    // or your red LED pin

  // Initialize the RFID module
  LOG_INFO("Initializing RFID Module...");
  setup_rfid_module();

  // Initialize Wi-Fi and setup the web server
  LOG_INFO("Initializing Web Server...");
  setup_webserver();

  LOG_INFO("Initializing Local Time...");
  timeSetup(); // Initialize local time

  // Initialize the door controller
  LOG_INFO("Initializing Door Controller...");
  DoorController::begin();

  // App Initialized
  LOG_INFO("Pet Feeder Initilization Completed!");
  LED_MSG_START();
}

void scan_id(){
  // Continuously scan for RFID tags
  String rfidUid = scan_rfid_card(); // Scan for RFID card and get UID

  // If no RFID UID is scanned, exit the function
  if ( rfidUid.length() < 1 ) {
    return;
  }

  LOG_INFO("RFID UID Scanned: " + rfidUid);
  LED_MSG_RFID_READ();

  // RFID is not allowed
  if ( !isAllowedRFID(rfidUid) ) { 
    LED_ERROR_RFID_INVALID();
    LOG_ERROR("RFIDManager", __FUNCTION__, "RFID not allowed: " + rfidUid);
    
    // Close the door if it is open
    DoorController::close();
    return;
  }

  // RFID is allowed
  LED_MSG_RFID_ALLOWED(); 
  DoorController::openWait();
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
