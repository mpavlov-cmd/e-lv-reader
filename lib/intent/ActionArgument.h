#ifndef ACTIONARGUMENT_H
#define ACTIONARGUMENT_H

#include <Arduino.h>
#include <lvgl.h>

struct ActionArgument
{
    static const ActionArgument NULL_ARG;

    lv_obj_t *target;
    lv_event_code_t code;
};

#endif