/*
 * Project: Pet Feeder - RFID-Authorized Access
 * Author: Guilherme Araujo aka Gartisk
 * Date: 24/06/2025
 * Version: 1.0.0
 * 
 */

#include "config.h" // Ensure this includes the LED_STATUS definition
#include "Useful.h"
#include "Log.h"
#include "Webserver.h" // Handles all web server functionalities
#include "RFIDManager.h"
#include "DoorController.h"
#include "SettingsManager.h" // Include LittleFS for file system operations

// Global variables that are shared across different modules
// These are defined here and extern'd in other .cpp files that need to access them.
String lastScannedRfidUID = "No scan yet"; // Stores the last scanned RFID UID for web display

DynamicJsonDocument rfidSettingsDoc(1024); // For parsing and holding RFID settings from web requests

void setup() {
  Serial.begin(SERIAL_BEGIN); // Initialize serial communication
  LOG_INFO("Pet Feeder - Setup Started");

  // Initialize the LED status
  ledStatus(5, 100);

  // Initialize LittleFS filesystem
  LOG_INFO("Settings Manager - Initializing");
  SettingsManager::begin(); 

  // Initialize the RFID module
  LOG_INFO("RFID Module - Initializing");
  setup_rfid_module();

  // Initialize Wi-Fi and setup the web server
  LOG_INFO("Web Server - Initializing");
  setup_webserver();

  // Initialize the door controller
  LOG_INFO("Door Controller - Initializing");
  door_begin();
}


void openDoor() {
  door_open(); // Open the door
  delay(1000); // Wait for 1 second
}

void loop() {

  // Continuously scan for RFID tags
  String rfidUid = scan_rfid_card(); // Scan for RFID card and get UID

  // TODO ONLY ALLOWS RFID LISTS
  if (rfidUid != "") {
    openDoor(); // If a valid RFID UID is scanned, open the door
  }

  // Handle incoming HTTP requests for the web server
  server.handleClient(); // Process incoming HTTP requests for the web server

  door_toggle();
  delay(500);
}
