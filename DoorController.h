#ifndef DOORCONTROLLER_H
#define DOORCONTROLLER_H

#include <Arduino.h>
#include <Servo.h>
#include "config.h"

enum DoorState { DOOR_CLOSED, DOOR_OPEN };

void door_begin();
void door_open();
void door_close();
void door_toggle();
DoorState door_get_state();

#endif // DOORCONTROLLER_H