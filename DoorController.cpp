#include "DoorController.h"

Servo DoorController::door_servo;
DoorState DoorController::door_state = DOOR_CLOSED;
unsigned long DoorController::lastButtonPressTime = 0;

int DoorController::targetAngle = 0;
int DoorController::moveStep = 5;
int DoorController::moveInterval = 20;
unsigned long DoorController::lastMoveTime = 0;
int DoorController::currentAngle = 0;

unsigned long DoorController::waitStartTime = 0;
unsigned long DoorController::waitTime = 0;
bool DoorController::waitingToClose = false;

void DoorController::begin() {
    door_servo.attach(DOOR_PIN);
    pinMode(DOOR_BTN_PIN, INPUT_PULLUP);
    close();

    lastButtonPressTime = 0;
    LOG_INFO("DoorController initialized.");
    LOG_INFO("Servo Pin: " + String(DOOR_PIN));
    LOG_INFO("Button Pin: " + String(DOOR_BTN_PIN));
    LOG_INFO("Open Angle: " + String(DOOR_OPEN_ANGLE));
    LOG_INFO("Close Angle: " + String(DOOR_CLOSED_ANGLE));
}

void DoorController::process() {
    unsigned long now = millis();

    // Handle servo movement
    if (door_state == DOOR_OPENING || door_state == DOOR_CLOSING) {
        if (now - lastMoveTime >= (unsigned long)moveInterval) {
            lastMoveTime = now;
            if (door_state == DOOR_OPENING && currentAngle < targetAngle) {
                currentAngle += moveStep;
                if (currentAngle > targetAngle) currentAngle = targetAngle;
                door_servo.write(currentAngle);
            } else if (door_state == DOOR_CLOSING && currentAngle > targetAngle) {
                currentAngle -= moveStep;
                if (currentAngle < targetAngle) currentAngle = targetAngle;
                door_servo.write(currentAngle);
            }

            // Check if movement finished
            if (currentAngle == targetAngle) {
                if (door_state == DOOR_OPENING) {
                    door_state = DOOR_OPEN;
                    LED_MSG_DOOR_OPEN();
                    LOG_INFO("Door opened.");
                    if (waitingToClose) {
                        waitStartTime = now;
                        door_state = DOOR_WAITING;
                    }
                } else if (door_state == DOOR_CLOSING) {
                    door_state = DOOR_CLOSED;
                    LED_MSG_DOOR_CLOSE();
                    LOG_INFO("Door closed.");
                    waitingToClose = false;
                }
            }
        }
    }

    // Handle waiting after open before closing
    if (door_state == DOOR_WAITING && waitingToClose) {
        if (now - waitStartTime >= waitTime) {
            close();
        }
    }
}

void DoorController::open() {
    int openAngle = SettingsManager::cached_settings["DOOR_OPEN_ANGLE"] | DOOR_OPEN_ANGLE;
    int openSpeed = SettingsManager::cached_settings["DOOR_OPEN_SPEED"] | DOOR_OPEN_SPEED;

    moveStep = 5;
    moveInterval = DOOR_DELAYER_DIVISOR / openSpeed;
    targetAngle = openAngle;
    currentAngle = door_servo.read();

    door_state = DOOR_OPENING;
    lastMoveTime = millis();
    waitingToClose = false;
    LOG_INFO("Door opening (non-blocking)...");
}

void DoorController::close() {
    int closeAngle = SettingsManager::cached_settings["DOOR_CLOSE_ANGLE"] | DOOR_CLOSED_ANGLE;
    int closeSpeed = SettingsManager::cached_settings["DOOR_CLOSE_SPEED"] | DOOR_CLOSE_SPEED;

    moveStep = 5;
    moveInterval = DOOR_DELAYER_DIVISOR / closeSpeed;
    targetAngle = closeAngle;
    currentAngle = door_servo.read();
    
    door_state = DOOR_CLOSING;
    lastMoveTime = millis();
    waitingToClose = false;
    
    // Reset wait time for closing
    LOG_INFO("Door closing (non-blocking)...");
}

void DoorController::open_close() {
    open();
    waitTime = SettingsManager::cached_settings["DOOR_CLOSE_WAIT"] | 10000;
    waitingToClose = true;
    // The rest is handled in process()
    LOG_INFO("Door open_close sequence started (non-blocking)...");
}

void DoorController::toggle() {
    int buttonState = digitalRead(DOOR_BTN_PIN);

    if (buttonState != LOW ) {
        return; // Button not pressed
    }

    if (!isDebounced(lastButtonPressTime, DOOR_DEBOUNCE_DELAY)) {
        return; // Ignore if button press is not debounced
    }

    // refresh last button press time
    // This is to prevent multiple toggles from a single press
    lastButtonPressTime = millis();
    
    if (door_state == DOOR_CLOSED) {
        LOG_INFO("Button pressed: Door is currently closed. Opening door.");
        open();
    } else if (door_state == DOOR_OPEN) {
        LOG_INFO("Button pressed: Door is currently open. Closing door.");
        close();
    }
    
}

DoorState DoorController::get_state() {
    return door_state;
}