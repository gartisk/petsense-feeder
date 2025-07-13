#ifndef DOORCONTROLLER_H
#define DOORCONTROLLER_H

#include <Arduino.h>
#include <Servo.h>
#include "config.h"
#include "Useful.h"
#include "Log.h"
#include "SettingsManager.h"

enum DoorState { DOOR_CLOSED, DOOR_OPEN, DOOR_OPENING, DOOR_CLOSING, DOOR_WAITING };

class DoorController {
public:
    static void begin();
    static void process(); // Call this in loop()
    static void open();
    static void close();
    static void open_close();
    static void toggle();
    static DoorState get_state();
    // static bool isDebounced();

private:
    static Servo door_servo;
    static DoorState door_state;
    static unsigned long lastButtonPressTime;

    // For non-blocking movement
    static int targetAngle;
    static int moveStep;
    static int moveInterval;
    static unsigned long lastMoveTime;
    static int currentAngle;

    // For open_close sequence
    static unsigned long waitStartTime;
    static unsigned long waitTime;
    static bool waitingToClose;
};

#endif // DOORCONTROLLER_H