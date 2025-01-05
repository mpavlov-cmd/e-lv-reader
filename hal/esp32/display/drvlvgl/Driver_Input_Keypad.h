#ifndef DRIVERINPUTKEYPAD_H
#define DRIVERINPUTKEYPAD_H

#include "lvgl.h"

// Used to assign widgets to group so defined as public

void lv_joystick_indev_init(void);
void set_lv_active_object(lv_obj_t* object);

lv_indev_t * get_lv_keypad(void);

#endif