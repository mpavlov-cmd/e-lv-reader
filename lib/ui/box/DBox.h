#ifndef DBOX_H
#define DBOX_H

#include <Arduino.h>

struct DBox
{
    static const DBox EMPTY;
    static const int16_t BOX_SCREEN_WIDTH  = 480;
    static const int16_t BOX_SCREEN_HEIGHT = 800;

    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
    uint8_t padding;
    uint8_t border;

    bool operator==(const DBox &other) const
    {
        return x == other.x &&
               y == other.y &&
               width == other.width &&
               height == other.height &&
               padding == other.padding &&
               border == other.border;
    }

    static DBox atCenter(uint16_t width, uint16_t height, uint8_t padding, uint8_t border)
    {
        DBox box;
        box.width = width;
        box.height = height;
        box.padding = padding;
        box.border = border;

        box.x = (BOX_SCREEN_WIDTH - width) / 2;
        box.y = (BOX_SCREEN_HEIGHT - height) / 2;
        return box;
    };

    static DBox stickTop(uint16_t width, uint16_t height, uint8_t padding, uint8_t border)
    {
        DBox box;
        box.width = width;
        box.height = height;
        box.padding = padding;
        box.border = border;

        box.x = (BOX_SCREEN_WIDTH - width) / 2;
        box.y = 0;
        return box;
    };

    static DBox stickBottom(uint16_t width, uint16_t height, uint8_t padding, uint8_t border)
    {
        DBox box;
        box.width = width;
        box.height = height;
        box.padding = padding;
        box.border = border;

        box.x = (BOX_SCREEN_WIDTH - width) / 2;
        box.y = BOX_SCREEN_HEIGHT - height;
        return box;
    };
};

#endif