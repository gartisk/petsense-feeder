#ifndef CONFIG_H
#define CONFIG_H

#include "LedBlinker.h"

// Pin Configuration Table
// +------+--------+-------------+-------------+-------------------------------------------------------------+
// | Label| GPIO   | Input       | Output      | Notes                                                       |
// +------+--------+-------------+-------------+-------------------------------------------------------------+
// | D0   | GPIO16 | no          | interrupt   | no PWM or I2C support, HIGH at boot, used to wake up from deep sleep |
// | D1   | GPIO5  | OK          | OK          | often used as SCL (I2C)                                     |
// | D2   | GPIO4  | OK          | OK          | often used as SDA (I2C)                                     |
// | D3   | GPIO0  | pulled up   | OK          | connected to FLASH button, boot fails if pulled LOW         |
// | D4   | GPIO2  | pulled up   | OK          | HIGH at boot, connected to on-board LED, boot fails if pulled LOW |
// | D5   | GPIO14 | OK          | OK          | SPI (SCLK)                                                  |
// | D6   | GPIO12 | OK          | OK          | SPI (MISO)                                                  |
// | D7   | GPIO13 | OK          | OK          | SPI (MOSI)                                                  |
// | D8   | GPIO15 | pulled GND  | OK          | SPI (CS), boot fails if pulled HIGH                         |
// | RX   | GPIO3  | OK          | RX pin      | HIGH at boot                                                |
// | TX   | GPIO1  | TX pin      | OK          | HIGH at boot, debug output at boot, boot fails if pulled LOW |
// | A0   | ADC0   | Analog Input| X           |                                                             |
// +------+--------+-------------+-------------+-------------------------------------------------------------+


// Component Pin Mapping Table
// +-------------+--------+-------------+-------------------+--------------------------------------------------------------+
// | Component   | GPIO   | NodeMCU Pin | Notes                                                |
// +-------------+--------+-------------+------------------------------------------------------+
// | RFID522 SDA | GPIO15 | D8          | Chip select (pulled LOW at boot, perfect use)        |
// | RFID522 SCK | GPIO14 | D5          | SPI Clock                                            |
// | RFID522 MOSI| GPIO13 | D7          | SPI Master Out                                       |
// | RFID522 MISO| GPIO12 | D6          | SPI Master In                                        |
// | RFID522 RST | GPIO0  | D3          | RST (use only at startup)                            |
// | RFID522 GND |   —    | —           | Connect to GND                                       |
// | RFID522 VCC |   —    | —           | Connect to 3.3V                                      |
// | Button      | GPIO16 | D0          | Input with INPUT_PULLUP, suitable for digital read    |
// | Servo       | GPIO5  | D1          | PWM capable, stable                                  |
// | Green LED   | GPIO4  | D2          | Standard digital output                              |
// | Red LED     | GPIO2  | D4          | OUTPUT safe (needs to be HIGH at boot, pull-up LED)  |
// +-------------+--------+-------------+------------------------------------------------------+

// Base Configuration
#define SERIAL_BEGIN 115200 // Baud rate for Serial Monitor

#define LED_STATUS 16        // ONBOARD LED (Usually GPIO2 on NodeMCU, connected to onboard LED)
#define LED_ERROR 2      // ERROR LED (Usually GPIO4 on NodeMCU, connected to an external LED)

#define SETTINGS_FILE_PATH "/settings.json" // File to store settings in LittleFS
#define SETTINGS_FILE_DEFAULT_PATH "/settings_default.json" // Path to the settings file in LittleFS
#define SETTINGS_FILE_SIZE 2048 // Size of the settings file in bytes (adjust as needed)

// Logging Configuration
#define LOG_LEVEL 4  // 0=NONE, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG
#define LOG_BUFFER_MAX_SIZE 4096  // Max log buffer size in chars (adjust as needed)


#define RFID_RST_PIN 0      // D3 = GPIO0 (Reset pin for MFRC522) 
#define RFID_SPI_PIN 15     // D8 = GPIO15 (SPI Slave Select pin for MFRC522)
#define RFID_SPI_DELAY 1000 // Delay after SPI bus initialization
#define RFID_DELAY 500      // Delay after MFRC522 PCD_Init
#define RFID_HISTORY_SIZE 10 // Number of last scanned RFID tags to keep in history

// Servo Module Configuration
#define DOOR_PIN 4        // Connect the servo's signal wire (usually yellow/orange) to this pin.
#define DOOR_BTN_PIN 5   // Connect one leg of your push button to this pin.
#define DOOR_DEBOUNCE_DELAY 50 // Debounce delay for the button in milliseconds.

#define DOOR_OPEN_ANGLE 180 // Example: Servo angle when the door is fully open.
#define DOOR_OPEN_SPEED 50 // Example: Speed of the door opening (in degrees per second).

#define DOOR_CLOSED_ANGLE 0 // Example: Servo angle when the door is fully closed.
#define DOOR_CLOSE_SPEED 50 // Example: Speed of the door closing (in degrees per
#define DOOR_CLOSE_WAIT 10000 // Example: Time to wait before closing the door after opening (in milliseconds).
#define DOOR_DELAYER_DIVISOR 1000 // Example: Divisor for the delay between servo movements (in milliseconds).

// WEB SERVER
#define WIFI_SETUP_RETRY_DELAY 1000
#define SERVER_PORT 80
#define SERVER_PAGE_MAIN "index.html"
#define SERVER_NOT_ALLOWED_FILES \
  { \
    "/settings.json" \
  }


// Function declaration only (definition must be in a .cpp file)
// void startLedBlink(int ledPin, int blinkingTimes, int ledDelay);
// void updateLedBlink();

// Example LED message macros (no semicolon at end)
#define LED_MSG_START()           GreenBlinker::start(5, 100)

#define LED_MSG_DOOR_OPEN()       GreenBlinker::start(1, 100)
#define LED_MSG_DOOR_CLOSE()      GreenBlinker::start(2, 100)

#define LED_MSG_RFID_READ()       GreenBlinker::start(1, 100)
#define LED_MSG_RFID_ALLOWED()    GreenBlinker::start(2, 100)

#define LED_ERROR_RFID_INVALID()  RedBlinker::start(2, 200)
#define LED_ERROR_RFID_DEVICE_NOT_FOUND() RedBlinker::start(3, 200)

#endif // CONFIG_H



