

#include "DoorController.h"

Servo DoorController::door_servo;
DoorState DoorController::door_state = DOOR_CLOSED;
unsigned long DoorController::lastButtonPressTime = 0;