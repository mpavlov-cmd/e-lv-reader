#ifndef MODELBOOKSTAT_H
#define MODELBOOKSTAT_H

#include <Arduino.h>
#include "box/DBox.h"
#include "lvgl.h"

struct ModelBookStat
{
    DBox box;
    uint16_t currentPage;
    uint16_t totalPages;
    const lv_font_t* lvFont;
};

#endif