#ifndef MODELCLOCK_H
#define MODELCLOCK_H

#include <Arduino.h>
#include <box/DBox.h>

struct ModelClock {

    DBox box;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;

};

#endif