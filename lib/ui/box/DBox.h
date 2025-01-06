#ifndef DBOX_H
#define DBOX_H

#include <Arduino.h>

struct DBox
{
    static const DBox EMPTY;

    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint8_t padding;
    uint8_t border;

    bool operator==(const DBox& other) const {
        return x == other.x &&
               y == other.y &&
               width == other.width &&
               height == other.height &&
               padding == other.padding &&
               border == other.border;
    }
};

#endif