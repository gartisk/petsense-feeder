
#include <ESP8266WebServer.h>
#include "config.h" // Include config.h for Wi-Fi credentials and other constants
#include "Webserver.h"
#include "wifi.h"
#include "Useful.h"
#include "Log.h"
#include "DoorController.h"
#include "FileManager.h" // Handles file system operations
#include "SettingsManager.h" // Manages settings storage and retrieval
#include "RFIDManager.h" // Handles RFID scanning and management

// Define the ESP8266WebServer instance here. It must only be defined once.
ESP8266WebServer server(SERVER_PORT);

// Extern declarations to access global variables defined in catfeeder.ino
// todo remove
extern String lastScannedRfidUID;

const char* root_path = "/";


// Determines the content type of a file based on its extension.
String getContentType(String filename) {
  // @TODO refine this function to use a map or array for better performance and maintainability.
  // Transform this to array map:
  if (server.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }

  return "text/plain";
}

// Handles requests for static files 
bool handleFileRequest(String path) {
  
  LOG_DEBUG("Webserver", __FUNCTION__, "Requested path: " + path);

  if (path.endsWith("/")) {
    path += SERVER_PAGE_MAIN; // Default to index.html for root or directory requests
  }

  String contentType = getContentType(path);
  String pathWithData = root_path + path; // LittleFS files are mounted under /

  if ( !FileManager::fileExists(pathWithData.c_str())) {
    return false;
  }

  File file = FileManager::open(pathWithData.c_str(), "r");
  if (!file) {
    return false;
  }

  server.streamFile(file, contentType); // Efficiently stream the file content
  file.close();
  return true;
}


// --- WEB SERVER HANDLERS ---

// Handler for 404 Not Found errors. Provides detailed error information.
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


// Handler for the root URL ("/"). Serves the index.html page.
void handleRoot() {
  String sPath = String("/") + SERVER_PAGE_MAIN;
  if (!handleFileRequest(sPath)) {
    server.send(404, "text/plain", "File Not Found");
  }
}

// Handles POST requests to "/save_settings" to update feeder settings.
// Parses JSON payload from the web page.
void handleSaveSettings() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"No data received\"}");
    return;
  }

  String body = server.arg("plain");
  Serial.println("Received POST request body for settings save:");
  Serial.println(body);

  StaticJsonDocument<SETTINGS_FILE_SIZE> doc;
  DeserializationError error = deserializeJson(doc, body);
  JsonObject obj = doc.as<JsonObject>();

  if (!SettingsManager::set(obj) || !SettingsManager::save()) {
    server.send(500, "application/json", "{\"status\":\"error\", \"message\":\"Failed to save settings\"}");
    return;
  } 

  server.send(200, "application/json", "{\"status\":\"success\", \"message\":\"Settings saved\"}");
}

// Handles GET requests to "/rfid_status" to provide real-time RFID data to the web page.
void handleLastRFIDs() {
  DynamicJsonDocument doc(512);
  JsonArray arr = doc.createNestedArray("rfids");

  for (const auto& rfid : lastRFIDs) {
    JsonObject obj = arr.createNestedObject();
    obj["timestamp"] = rfid.timestamp;
    obj["id"] = rfid.id;
  }

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleToggleDoor() {
  LOG_INFO("Toggle Door Request Received");
  
  if (DoorController::get_state() == DOOR_CLOSED) {
    DoorController::open(); // Open the door if it's currently closed
    server.send(200, "application/json", "{\"status\":\"success\", \"message\":\"Opening the Door\", \"door_state\": \"open\"}");
    return;
  }

  DoorController::close(); // Close the door if it's currently open
  server.send(200, "application/json", "{\"status\":\"success\", \"message\":\"Closing the Door\", \"door_state\": \"closed\"}");
}

void handleDoorState() {
  String state = (DoorController::get_state() == DOOR_OPEN) ? "open" : "closed";
  server.send(200, "application/json", "{\"door_state\":\"" + state + "\"}");
}

void handleSettingsRead() {
  StaticJsonDocument<SETTINGS_FILE_SIZE> settingsDoc;
  JsonObject obj = settingsDoc.to<JsonObject>();

  if (!SettingsManager::get(obj)) {
    server.send(500, "application/json", "{\"status\":\"error\", \"message\":\"Failed to read settings\"}");
    return;
  } 

  String jsonResponse;
  serializeJson(obj, jsonResponse);
  server.send(200, "application/json", jsonResponse);
}


void handleSettingsUpdate() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"No data received\"}");
    return;
  }

  String body = server.arg("plain");
  Serial.println("Received POST request body for settings update:");
  Serial.println(body);

  StaticJsonDocument<SETTINGS_FILE_SIZE> doc;
  DeserializationError error = deserializeJson(doc, body);
  JsonObject obj = doc.as<JsonObject>();

  if (!SettingsManager::set(obj)) {
    server.send(500, "application/json", "{\"status\":\"error\", \"message\":\"Failed to update settings\"}");
    return;
  } 

  server.send(200, "application/json", "{\"status\":\"success\", \"message\":\"Settings updated\"}");
}

void handleSettingsSave() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"status\":\"error\", \"message\":\"No data received\"}");
    return;
  }

  String body = server.arg("plain");
  Serial.println("Received POST request body for settings save:");
  Serial.println(body);

  StaticJsonDocument<SETTINGS_FILE_SIZE> doc;
  DeserializationError error = deserializeJson(doc, body);
  JsonObject obj = doc.as<JsonObject>();

  if (SettingsManager::set(obj) && SettingsManager::save() ) {
    server.send(200, "application/json", "{\"status\":\"success\", \"message\":\"Settings saved\"}");
  } 
  
  server.send(500, "application/json", "{\"status\":\"error\", \"message\":\"Failed to save settings\"}");
  return;
}

// Handles GET requests to "/logs" to provide the log buffer content.
void handleLogs() {
  server.send(200, "text/plain", logBuffer); // Send logs as plain text
}

// Initializes Wi-Fi connection and sets up all web server routes.
void setup_webserver () {
  Serial.print("Connecting to WiFi ");
  Serial.println(ssid); // ssid from config.h
  WiFi.begin(ssid, password); // password from config.h
  
  // Initialize LittleFS filesystem
  // if (!LittleFS.begin()) {
  //     Serial.println("LittleFS mount failed!");
  // }
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/logs", HTTP_GET, handleLogs);
  server.on("/api/toggle_door", HTTP_POST, handleToggleDoor);
  server.on("/api/door_state", HTTP_GET, handleDoorState);
  server.on("/api/last_rfids", HTTP_GET, handleLastRFIDs);
  
  server.on("/api/settings", HTTP_GET, handleSettingsRead);
  server.on("/api/settings", HTTP_PUT, handleSettingsUpdate);
  server.on("/api/settings", HTTP_POST, handleSettingsSave);
  
  // server.on("/api/settings/restore", HTTP_POST, handleRestoreSettings);
  
  // Set up a generic handler for any unmapped file requests (e.g., /style.css)
  server.onNotFound([]() {
    if (!handleFileRequest(server.uri())) {
      handleNotFound(); // If file not found in LittleFS, then return 404
    }
  });

  server.begin(); // Start the web server
  Serial.println("HTTP server started");
}
