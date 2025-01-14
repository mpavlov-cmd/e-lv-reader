#ifndef MODELSTATUS_H
#define MODELSTATUS_H

#include <Arduino.h>
#include "box/DBox.h"
#include "lvgl.h"

struct ModelStatus
{
    DBox box;
    bool plugged;
    bool charging;
    uint8_t batteryLevel;
    String time;
    char extra[32];
    
    const lv_font_t* lvFont;
};

#endif