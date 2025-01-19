#ifndef DRIVERINPUTKEYPAD_H
#define DRIVERINPUTKEYPAD_H

#include "lvgl.h"

void lv_joystick_indev_init(void);
unsigned long lv_joystick_last_hit(void);

lv_indev_t* lv_get_keypad(void);

#endif