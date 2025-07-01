// DoorController.cpp
#include "DoorController.h" // Include the custom header file
#include "Useful.h"           // Include the useful functions header for logging
#include "Log.h"

static Servo door_servo;
static DoorState door_state = DOOR_CLOSED;
static unsigned long lastButtonPressTime = 0;
static const unsigned long debounceDelay = 200; // ms

// begin() method implementation
// This function sets up the hardware connections and initial state.
void door_begin() {
    door_servo.attach(DOOR_PIN);
    door_servo.write(DOOR_CLOSED_ANGLE);
    pinMode(DOOR_BTN_PIN, INPUT_PULLUP);
    door_state = DOOR_CLOSED;
    lastButtonPressTime = 0;

    Serial.println("DoorController initialized.");
    Serial.print("Servo Pin: "); Serial.println(DOOR_PIN);
    Serial.print("Button Pin: "); Serial.println(DOOR_BTN_PIN);
    Serial.print("Open Angle: "); Serial.println(DOOR_OPEN_ANGLE);
    Serial.print("Close Angle: "); Serial.println(DOOR_CLOSED_ANGLE);
}

// openDoor() method implementation
// Moves the servo to the angle defined as the "open" position.
void door_open() {
    door_servo.write(DOOR_OPEN_ANGLE);
    door_state = DOOR_OPEN;
    Serial.println("Door opened.");
}

// closeDoor() method implementation
// Moves the servo to the angle defined as the "closed" position.
void door_close() {
    door_servo.write(DOOR_CLOSED_ANGLE);
    door_state = DOOR_CLOSED;
    Serial.println("Door closed.");
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
