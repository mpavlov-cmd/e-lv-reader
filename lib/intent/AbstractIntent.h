#ifndef ABSTRACTINTENT_H
#define ABSTRACTINTENT_H

#pragma once

#include <Arduino.h>
#include <IntentArgument.h>
#include <ActionArgument.h>
#include "lvgl.h"
#include "drvlvgl/Driver_Input_Keypad.h"
#include "esp_log.h"
#include "LogTags.h"

enum ActionRetultType { VOID, CHANGE_INTENT };

struct ActionResult {
    static const ActionResult VOID;

    ActionRetultType type;
    uint8_t id;
    IntentArgument data;
};

struct AbstractIntent {

    protected:
        // Intialized on stratup
        lv_group_t* widgetGroup;
        QueueHandle_t& eventQueue;

    public:
        virtual void onStartUp(IntentArgument) = 0;
        virtual void onFrequncy() = 0;
        virtual void onExit() = 0;

        virtual ActionResult onAction(ActionArgument) = 0;
        virtual uint8_t getId() = 0;

        AbstractIntent(QueueHandle_t& mEventQueue);
        virtual ~AbstractIntent() {
            if (widgetGroup != nullptr) {
                lv_group_del(widgetGroup);
            }
        } 
};

#endif