

#include "DoorController.h"
#include "Useful.h"
#include "Log.h"
#include "SettingsManager.h"

Servo DoorController::door_servo;
DoorState DoorController::door_state = DOOR_CLOSED;
unsigned long DoorController::lastButtonPressTime = 0;

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

void DoorController::servo_move_to_angle(int angle, int speed) {
    int currentAngle = door_servo.read();
    if (currentAngle == angle) return;
    int step = ((angle > currentAngle) ? 1 : -1) * 10;
    int delayResult = DOOR_DELAYER_DIVISOR / speed;
    if (currentAngle < angle) {
        for (int pos = currentAngle; pos <= angle; pos += step) {
            door_servo.write(pos);
            delay(delayResult);
        }
    } else {
        for (int pos = currentAngle; pos >= angle; pos += step) {
            door_servo.write(pos);
            delay(delayResult);
        }
    }
    door_servo.write(angle);
}

void DoorController::open() {
    int openAngle = SettingsManager::cached_settings["DOOR_OPEN_ANGLE"].as<int>();
    int openSpeed = SettingsManager::cached_settings["DOOR_OPEN_SPEED"].as<int>();
    servo_move_to_angle(openAngle, openSpeed);
    door_state = DOOR_OPEN;
    LOG_INFO("Door opened. " + String(openAngle) + " degrees." + String(openSpeed) + " ms.");
}

void DoorController::close() {
    int closeAngle = SettingsManager::cached_settings["DOOR_CLOSE_ANGLE"].as<int>();
    int closeSpeed = SettingsManager::cached_settings["DOOR_CLOSE_SPEED"].as<int>();
    servo_move_to_angle(closeAngle, closeSpeed);
    door_state = DOOR_CLOSED;
    LOG_INFO("Door closed. " + String(closeAngle) + " degrees." + String(closeSpeed) + " ms.");
}

bool DoorController::isDebounced() {
    return (millis() - lastButtonPressTime) > debounceDelay;
}

void DoorController::open_close() {
    open();
    delay(SettingsManager::cached_settings["DOOR_OPEN_SPEED"].as<int>());
    close();
    LOG_INFO("Door opened and closed in sequence.");
}

void DoorController::toggle() {
    int buttonState = digitalRead(DOOR_BTN_PIN);
    if (buttonState == LOW && isDebounced()) {
        lastButtonPressTime = millis();
        if (door_state == DOOR_CLOSED) {
            LOG_INFO("Button pressed: Door is currently closed. Opening door.");
            open();
        } else {
            LOG_INFO("Button pressed: Door is currently open. Closing door.");
            close();
        }
    }
}

DoorState DoorController::get_state() {
    return door_state;
}
