#ifndef INTENTHOME_H
#define INTENTHOME_H

#pragma once

#include <esp_log.h>
#include <LogTags.h>
#include <AbstractIntent.h>
#include <ESP32Time.h>
#include <FileManager.h>
#include <widget/WidgetMenu.h>
#include <model/menu/Menu.h>
#include <ButtonActions.h>
#include <IntentIdentifier.h>

struct IntentHome : public AbstractIntent
{
    private:

        ESP32Time& espTime;
        FileManager& fileManager;

        lv_obj_t* menuParent = nullptr;
        WidgetMenu* widgetMenu = nullptr;

        // Main menu and clock models
        Menu* menu = nullptr;
        DBox* menuBox = nullptr;

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
            delete menuBox;
            delete widgetMenu;
            if (lv_obj_is_valid(menuParent)) {
    			lv_obj_del(menuParent);
            }
            ESP_LOGD(TAG_INTNT, "IntentHome Destructor End");
        };
};

#endif
