#ifndef MODELCLOCK_H
#define MODELCLOCK_H

#include <Arduino.h>
#include <box/DBox.h>
#include <ESP32Time.h>

struct ModelClock {

    DBox box;
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;

    static void updateWithEspTime(ModelClock& modelClock, ESP32Time& espTime) {
        modelClock.year  = (uint16_t) espTime.getYear();
        modelClock.month = (uint8_t) (espTime.getMonth() + 1);
        modelClock.day   = (uint8_t) espTime.getDay();
        modelClock.hour  = (uint8_t) espTime.getHour(true);
        modelClock.min   = (uint8_t) espTime.getMinute();
        modelClock.sec   = (uint8_t) espTime.getSecond();
    };
};

#endif