#ifndef MODELTEXT_H
#define MODELTEXT_H

#include <Arduino.h>
#include "lvgl.h"
#include <model/AbstractModel.h>
#include <box/DBox.h>

struct ModelText {
    DBox box;
    String text;
    const lv_font_t* font;
    lv_align_t align;
    bool hasAction;

    static ModelText* newPtr() {
        return new ModelText {DBox::EMPTY, "Text", &lv_font_montserrat_14, LV_ALIGN_TOP_LEFT, false};
    }
};

#endif