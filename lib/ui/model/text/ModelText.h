#ifndef MODELTEXT_H
#define MODELTEXT_H

#include <Arduino.h>
#include <model/AbstractModel.h>
#include <model/DPosition.h>
#include <box/DBox.h>

// TODO: Add properties wether the object should be added to event group
// TODO: Add font settings and text alignment settings 
struct ModelText {
    DBox box;
    String text;
};

#endif