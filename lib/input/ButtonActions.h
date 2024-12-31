#ifndef BUTTONACTIONS_H
#define BUTTONACTIONS_H

#include <Arduino.h>

#define BUTTON_ACTION_LEFT  B00000010
#define BUTTON_ACTION_DOWN  B00000001
#define BUTTON_ACTION_RIGHT B00000100
#define BUTTON_ACTION_UP    B00000011
#define BUTTON_ACTION_MID   B00001000

uint8_t controlDirection(uint8_t input, bool& held);

#endif