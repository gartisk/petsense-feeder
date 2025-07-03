#ifndef DOORCONTROLLER_H
#define DOORCONTROLLER_H

#include <Arduino.h>
#include <Servo.h>
#include "config.h"


enum DoorState { DOOR_CLOSED, DOOR_OPEN };

class DoorController {
public:
    static void begin();
    static void open();
    static void close();
    static void toggle();
    static void open_close();
    static DoorState get_state();

private:
    static Servo door_servo;
    static DoorState door_state;
    static unsigned long lastButtonPressTime;
    static constexpr unsigned long debounceDelay = 200;
    static void servo_move_to_angle(int angle, int speed);
    static bool isDebounced();
};

#endif // DOORCONTROLLER_H