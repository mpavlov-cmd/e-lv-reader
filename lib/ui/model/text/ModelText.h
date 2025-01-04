#ifndef MODELTEXT_H
#define MODELTEXT_H

#include <Arduino.h>
#include <model/AbstractModel.h>
#include <model/DPosition.h>
#include <box/DBox.h>

struct ModelText {
    DBox box;
    DPosition position;
    String text;
};

#endif