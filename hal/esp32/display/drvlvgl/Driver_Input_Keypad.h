#ifndef DRIVERINPUTKEYPAD_H
#define DRIVERINPUTKEYPAD_H

#include "lvgl.h"

void lv_joystick_indev_init(void);
void lv_joystick_invalidate(bool enabled);
void lv_joystick_active_object(lv_obj_t* object);

lv_indev_t* lv_get_keypad(void);

#endif