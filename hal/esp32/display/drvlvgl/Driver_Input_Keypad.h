#ifndef DRIVERINPUTKEYPAD_H
#define DRIVERINPUTKEYPAD_H

#include "lvgl.h"

void lv_joystick_indev_init(void);
lv_indev_t* lv_get_keypad(void);

#endif