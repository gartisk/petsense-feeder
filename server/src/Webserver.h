#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <ESP8266WebServer.h> // Required for ESP8266 web server functionality
// #include <LittleFS.h>         // Required for accessing files from LittleFS
#include <ArduinoJson.h>      // Required for JSON parsing and serialization

#include "FileManager.h"              // Required for file system operations

// Declare the ESP8266WebServer instance as 'extern'
// This tells the compiler that 'server' is defined elsewhere (in webserver_handlers.cpp)
extern ESP8266WebServer server;


// Function declarations for web server-related operations
String getContentType(String filename);
bool handleFileRequest(String path);
void handleRoot();
void handleNotFound();
void handleRfidStatus();

// Unified function to set up Wi-Fi and web server routes
void setup_webserver();

#endif // WEBSERVER_H
