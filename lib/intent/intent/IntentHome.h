#ifndef INTENTHOME_H
#define INTENTHOME_H

#pragma once

#include <esp_log.h>
#include <LogTags.h>
#include <AbstractIntent.h>
#include <ESP32Time.h>
#include <FileManager.h>

#include <IntentIdentifier.h>
#include <widget/WidgetMenu.h>
#include <model/menu/Menu.h>
#include <widget/WidgetClock.h>
#include <model/clock/ModelClock.h>

struct IntentHome : public AbstractIntent
{
    private:

        ESP32Time& espTime;
        FileManager& fileManager;

        DBox boxMenu {48, 456, 384, 216, 8, 0};
        DBox boxClock = DBox::atCenter(256, 128, 8, 0);

        lv_obj_t* menuParent = nullptr;
        WidgetMenu* widgetMenu = nullptr;
        WidgetClock* widgetClock = nullptr;

        // Main menu and clock models
        Menu* menu = nullptr;
        ModelClock* modelClock = nullptr;

        // Last inute to run on requency only if time changes
        int lastMinute = 0;

        void updateTime();

    public:
        // Constant declaration
        static constexpr const uint8_t ID = INTENT_ID_HOME;

        IntentHome(QueueHandle_t& mEventQueue, ESP32Time& espTime, FileManager& fm);

        void onStartUp(IntentArgument arg) override;
        void onFrequncy() override;
        void onExit() override;

        ActionResult onAction(ActionArgument arg) override;
        uint8_t getId() override;

        ~IntentHome() {
            ESP_LOGD(TAG_INTNT, "IntentHome Destructor Start");
            delete menu;
            delete widgetMenu;
            delete modelClock;
            delete widgetClock;
            if (lv_obj_is_valid(menuParent)) {
    			lv_obj_del(menuParent);
            }
            ESP_LOGD(TAG_INTNT, "IntentHome Destructor End");
        };
};

#endif
