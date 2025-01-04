#ifndef DBOX_H
#define DBOX_H

#include <Arduino.h>

struct DBox
{
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint8_t padding;
    uint8_t border;
};

#endif