// DoorController.cpp
#include "DoorController.h" // Include the custom header file
#include "Useful.h"           // Include the useful functions header for logging
#include "Log.h"
#include "SettingsManager.h" // Include the settings manager for configuration

static Servo door_servo;
static DoorState door_state = DOOR_CLOSED;
static unsigned long lastButtonPressTime = 0;
static const unsigned long debounceDelay = 200; // ms

// begin() method implementation
// This function sets up the hardware connections and initial state.
void door_begin() {
    door_servo.attach(DOOR_PIN, 500, 2400);
    pinMode(DOOR_BTN_PIN, INPUT_PULLUP); // Set button pin as input
    
    door_close(); // Initialize the door to the closed position
    lastButtonPressTime = 0;

    LOG_INFO("DoorController initialized.");
    LOG_INFO("Servo Pin: " + String(DOOR_PIN));
    LOG_INFO("Button Pin: " + String(DOOR_BTN_PIN));
    LOG_INFO("Open Angle: " + String(DOOR_OPEN_ANGLE));
    LOG_INFO("Close Angle: " + String(DOOR_CLOSED_ANGLE));
}

// void servo_move_to_angle(int angle, int speed) {
//     int currentAngle = door_servo.read();
//     if (currentAngle == angle) return;

//     int step = (angle > currentAngle) ? 1 : -1;
//     int delayPerStep = speed > 0 ? speed : 10; // fallback to 10ms if speed is 0

//     for (int pos = currentAngle; pos != angle; pos += step) {
//         door_servo.write(pos);
//         delay(delayPerStep);
//     }
//     door_servo.write(angle);
// }

void servo_move_to_angle(int angle, int speed) {
    int currentAngle = door_servo.read();
    
    if (currentAngle == angle) return;

    int step = ((angle > currentAngle) ? 1 : -1)*10;

    int delayResult = DOOR_DELAYER_DIVISOR / speed;

    if( currentAngle < angle ){
        // open
        for (int pos = currentAngle; pos <= angle; pos += step) {
            door_servo.write(pos);
            delay(delayResult);
        }
    } else {
        //close
        for (int pos = currentAngle; pos >= angle; pos += step) {
            door_servo.write(pos);
            delay(delayResult);
        }
    }


    door_servo.write(angle);
}


// openDoor() method implementation
// Moves the servo to the angle defined as the "open" position.
void door_open() {
    int openAngle = SettingsManager::cached_settings["DOOR_OPEN_ANGLE"].as<int>();
    int openSpeed = SettingsManager::cached_settings["DOOR_OPEN_SPEED"].as<int>();

    servo_move_to_angle(openAngle, openSpeed);    
    door_state = DOOR_OPEN;

    LOG_INFO("Door opened. " + String(openAngle) + " degrees." + String(openSpeed) + " ms.");
}

// closeDoor() method implementation
// Moves the servo to the angle defined as the "closed" position.
void door_close() {
    int closeAngle = SettingsManager::cached_settings["DOOR_CLOSE_ANGLE"].as<int>();
    int closeSpeed = SettingsManager::cached_settings["DOOR_CLOSE_SPEED"].as<int>();
    
    servo_move_to_angle(closeAngle, closeSpeed);
    door_state = DOOR_CLOSED;

    LOG_INFO("Door closed. " + String(closeAngle) + " degrees." + String(closeSpeed) + " ms.");
}

static bool isDebounced() {
    return (millis() - lastButtonPressTime) > debounceDelay;
}

// toggleDoor() method implementation
// Checks the button state and toggles the door if the button is pressed and debounced.
void door_toggle() {
    int buttonState = digitalRead(DOOR_BTN_PIN);
    if (buttonState == LOW && isDebounced()) {
        lastButtonPressTime = millis();
        if (door_state == DOOR_CLOSED) {
            LOG_INFO("Button pressed: Door is currently closed. Opening door.");
            door_open();
        } else {
            LOG_INFO("Button pressed: Door is currently open. Closing door.");
            door_close();
        }
    }
}


// getDoorState() method implementation
// Returns the current state of the door.
DoorState door_get_state() {
    return door_state;
}
