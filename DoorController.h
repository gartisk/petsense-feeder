#ifndef DOORCONTROLLER_H
#define DOORCONTROLLER_H

#include <Arduino.h>
#include <Servo.h>
#include "config.h"
#include "Useful.h"
#include "Log.h"
#include "SettingsManager.h"

enum DoorState { DOOR_CLOSED, DOOR_OPEN };

class DoorController {
public:

    static void begin() {
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

    // Move the servo to the specified angle at the specified speed.
    // The angle should be between 0 and 180 degrees.
    // The speed is in milliseconds for the full movement.
    static void servo_move_to_angle(int angle, int speed, DoorState state) {
        
        int currentAngle = door_servo.read();
        if (currentAngle == angle) return;

        int step = 5;
        int delayInterval = (int) ceil(DOOR_DELAYER_DIVISOR / speed);

        if( state == DOOR_OPEN){
            for (int pos = currentAngle; pos < angle; pos += step) {
                door_servo.write(pos);
                delay(delayInterval);
            }

        } else {
            for (int pos = currentAngle; pos > angle; pos -= step) {
                door_servo.write(pos);
                delay(delayInterval);
            }
        }

        door_servo.write(angle); // Ensure the servo ends at the exact angle
    }

    static void open() {
        int openAngle = SettingsManager::cached_settings["DOOR_OPEN_ANGLE"].as<int>();
        int openSpeed = SettingsManager::cached_settings["DOOR_OPEN_SPEED"].as<int>();
        servo_move_to_angle(openAngle, openSpeed, DOOR_OPEN);
        door_state = DOOR_OPEN;
        LOG_INFO("Door opened. " + String(openAngle) + " degrees." + String(openSpeed) + " ms.");
    }

    static void close() {
        int closeAngle = SettingsManager::cached_settings["DOOR_CLOSE_ANGLE"].as<int>();
        int closeSpeed = SettingsManager::cached_settings["DOOR_CLOSE_SPEED"].as<int>();
        servo_move_to_angle(closeAngle, closeSpeed, DOOR_CLOSED);
        door_state = DOOR_CLOSED;
        LOG_INFO("Door closed. " + String(closeAngle) + " degrees." + String(closeSpeed) + " ms.");
    }

    static bool isDebounced() {
        return (millis() - lastButtonPressTime) > DOOR_DEBOUNCE_DELAY;
    }

    // Opens the door, waits for the specified open speed, then closes it.
    static void open_close() {
        open();
        delay(SettingsManager::cached_settings["DOOR_CLOSE_WAIT"].as<int>());
        close();
        LOG_INFO("Door opened and closed in sequence.");
    }

    static void toggle() {
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

    static DoorState get_state() {
        return door_state;
    }

private:
    static Servo door_servo;
    static DoorState door_state;
    static unsigned long lastButtonPressTime;
};

#endif // DOORCONTROLLER_H