#include "config.h" 
#include "SettingsManager.h"
#include "Log.h"
#include "Useful.h"
#include "LocalTime.h" // Handles local time functionalities

#include "Webserver.h" // Handles all web server functionalities
#include "RFIDManager.h"
#include "DoorController.h"
#include "LedBlinker.h" // Handles LED blinking functionalities

// Button debouncing variables
unsigned long lastButtonPress = 0;
int lastButtonState = HIGH;
int currentButtonState = HIGH;

// Time sync variables
unsigned long lastTimeSync = 0;
const unsigned long TIME_SYNC_INTERVAL = 3600000; // Sync every hour (3600000ms)


void setup() {
  Serial.begin(SERIAL_BEGIN); // Initialize serial communication
  LOG_INFO("Pet Feeder - Setup");

  // Initialize LittleFS filesystem
  LOG_INFO("Initializing Settings Manager...");
  SettingsManager::begin(); 
  SettingsManager::load(); 

  // Initialize LEDs
  LOG_INFO("Initializing LEDs...");
  GreenBlinker::begin(LED_STATUS_PIN); // or your green LED pin
  RedBlinker::begin(LED_ERROR_PIN);    // or your red LED pin

  // Initialize the RFID module
  LOG_INFO("Initializing RFID Module...");
  setup_rfid_module();

  // Initialize Wi-Fi and setup the web server
  LOG_INFO("Initializing Web Server...");
  setup_webserver();

  LOG_INFO("Initializing Local Time...");
  timeSetup(); // Initialize local time
  LOG_INFO(String("Current Local Time: ") + getLocalTime()); // get the current local time

  // Initialize the door controller
  LOG_INFO("Initializing Door Controller...");
  DoorController::begin();

  // Initialize the door button
  LOG_INFO("Initializing Door Button...");
  pinMode(DOOR_BTN_PIN, INPUT_PULLUP);

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

void scan_btn(){
    // Read the current button state
    currentButtonState = digitalRead(DOOR_BTN_PIN);
    
    // this avoid opening and closing sequentially without unpressing the button
    if (currentButtonState == lastButtonState) {
      return; // Exit if the button state has not changed
    }

    lastButtonState = currentButtonState;
    
    unsigned long currentTime = millis();
    bool isBtnDebounced = currentTime - lastButtonPress >= DOOR_DEBOUNCE_DELAY;
    
    // Button Debouncing check
    if (!isBtnDebounced) {
        return;
    }

    DoorController::toggle();
    lastButtonPress = currentTime;
}

void updateTimeSync() {
    unsigned long currentTime = millis();
    
    // Check if it's time to sync (every hour)
    if (currentTime - lastTimeSync >= TIME_SYNC_INTERVAL) {
        LOG_INFO("Syncing time with NTP server...");
        
        // Force NTP sync
        configTime(NTP_TIMEZONE_UTC * NTP_TIMEZONE_OFFSET, 0, NTP_SERVER_ADDRESS, NTP_SERVER_ADDRESS_ALT);
        
        // Wait a bit for sync to complete
        delay(100);
        
        LOG_INFO(String("Time synced - Current Local Time: ") + getLocalTime());
        lastTimeSync = currentTime;
    }
}

void loop() {
  // Call the function to scan for RFID tags
  scan_id(); 
  
  // Handle incoming HTTP requests for the web server
  // Process incoming HTTP requests for the web server
  server.handleClient(); 
  
  GreenBlinker::update();
  RedBlinker::update();

  DoorController::process();
  scan_btn(); // Check if the door button is pressed
  
  // Update time sync periodically
  updateTimeSync();
}
