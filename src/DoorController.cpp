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
    door_servo.attach(DOOR_PIN, 500, 2400); // Attach the servo to the specified pin with min and max pulse width
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

    bool isOpening = door_state == DOOR_OPENING;
    bool isClosing = door_state == DOOR_CLOSING;
    bool isWaiting = (door_state == DOOR_WAITING && waitingToClose);
   
    if ( !isOpening && !isClosing && !isWaiting)  {
        return;
    }

    bool isOpeningOrClosingDebounced = isDebounced(lastMoveTime, moveInterval, now);    
    bool isWaitingDebounced = isDebounced(waitStartTime, waitTime, now);

    if ( isOpeningOrClosingDebounced ){
            
        // Open the Door
        if( isOpening && currentAngle < targetAngle ) {
            lastMoveTime = now; // Update last move time
            currentAngle += moveStep;
            
            if (currentAngle > targetAngle) currentAngle = targetAngle;
            door_servo.write(currentAngle); // Write the current angle to the servo

        // Close the Door
        } else if( isClosing && currentAngle > targetAngle ) {
            lastMoveTime = now; // Update last move time
            currentAngle -= moveStep;

            if (currentAngle < targetAngle) currentAngle = targetAngle;
            door_servo.write(currentAngle); // Write the current angle to the servo
        }

        //  Movement Finished
        if (currentAngle == targetAngle) {
            if ( isOpening ) {
                door_state = DOOR_OPEN;
                LED_MSG_DOOR_OPEN();
                LOG_INFO("Door opened.");
                if (waitingToClose) {
                    waitStartTime = now;
                    door_state = DOOR_WAITING;
                }
            } else if ( isClosing) {
                door_state = DOOR_CLOSED;
                LED_MSG_DOOR_CLOSE();
                LOG_INFO("Door closed.");
                waitingToClose = false;
            }
        }
    }

        // Handle waiting after open before closing
    if ( isWaiting && waitingToClose && isWaitingDebounced) {
        close();
    }
}

void DoorController::open() {
    JsonObject settings = SettingsManager::cached_settings.as<JsonObject>();
    LOG_INFO(String(SettingsManager::cached_settings.as<String>()));
    
    // is null or empty, use default values
    int openAngle = settings["DOOR_OPEN_ANGLE"] != "" ? settings["DOOR_OPEN_ANGLE"] : DOOR_OPEN_ANGLE;
    int openSpeed = settings["DOOR_OPEN_SPEED"] != "" ? settings["DOOR_OPEN_SPEED"] : DOOR_OPEN_SPEED;

    moveStep = 5;
    moveInterval = DOOR_DELAYER_DIVISOR / openSpeed;
    targetAngle = openAngle;
    currentAngle = door_servo.read();

    door_state = DOOR_OPENING;
    lastMoveTime = millis();
    waitingToClose = false;
    LOG_INFO("Door Opening - Angle " + String(targetAngle) + " Speed " + String(openSpeed) + " MoveInterval " + String(moveInterval) + " (non-blocking)...");
}

void DoorController::close() {
    JsonObject settings = SettingsManager::cached_settings.as<JsonObject>();

    int closeAngle = settings["DOOR_CLOSED_ANGLE"] != "" ? settings["DOOR_CLOSED_ANGLE"] : DOOR_CLOSED_ANGLE;
    int closeSpeed = settings["DOOR_CLOSE_SPEED"] != "" ? settings["DOOR_CLOSE_SPEED"] : DOOR_CLOSE_SPEED;

    moveStep = 5;
    moveInterval = DOOR_DELAYER_DIVISOR / closeSpeed;
    targetAngle = closeAngle;
    currentAngle = door_servo.read();
    
    door_state = DOOR_CLOSING;
    lastMoveTime = millis();
    waitingToClose = false;
    
    // Reset wait time for closing
    LOG_INFO("Door Closing - Angle " + String(targetAngle) + " Speed " + String(closeSpeed) + " MoveInterval " + String(moveInterval) + " (non-blocking)...");
}

void DoorController::open_close() {
    open();
    JsonObject settings = SettingsManager::cached_settings.as<JsonObject>();
    waitTime = settings["DOOR_CLOSE_WAIT"] != "" ? settings["DOOR_CLOSE_WAIT"] : DOOR_CLOSE_WAIT; // Default to 10 seconds if not set

    lastMoveTime = millis();
    waitingToClose = true;
    // The rest is handled in process()
    LOG_INFO("Door open_close sequence started (non-blocking)...");
}

void DoorController::toggle() {
    int buttonState = digitalRead(DOOR_BTN_PIN);

    if (buttonState != LOW ) {
        return; // Button not pressed
    }

    bool isCompletedDebounced = (millis() - lastButtonPressTime) > DOOR_DEBOUNCE_DELAY;
    
    if( !isCompletedDebounced ) {
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