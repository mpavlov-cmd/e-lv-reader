#ifndef DRIVERINPUTKEYPAD_H
#define DRIVERINPUTKEYPAD_H

#include "lvgl.h"

// Used to assign widgets to group so defined as public

void lv_joystick_indev_init(void);
lv_indev_t * get_lv_keypad(void);

#endif