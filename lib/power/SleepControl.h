#ifndef SLEEPCONTROL_H
#define SLEEPCONTROL_H

#include <Arduino.h>

struct SleepControlConf
{
    uint64_t gpioMask;
    esp_sleep_ext1_wakeup_mode_t mode;
};


struct SleepControl 
{
    private:
        const int US_TO_S_FACTOR = 1000000;

        SleepControlConf config;
        esp_err_t extWakeUpConfResult;

    public:
        SleepControl(SleepControlConf config);

        void sleepNow();
        bool setWarkupTimer(uint16_t wakeupTimerSecons);
        void configureExt1WakeUp();
        esp_sleep_source_t getWarkeupCause();
};

#endif