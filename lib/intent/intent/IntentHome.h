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

        WidgetMenu* widgetMenu = nullptr;

        // Main menu and clock models
        Menu* menu = nullptr;
        DBox* menuBox = nullptr;


    public:
        // Constant declaration
        static constexpr const uint8_t ID = INTENT_ID_HOME;

        IntentHome(ESP32Time& espTime, FileManager& fm);

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
            ESP_LOGD(TAG_INTNT, "IntentHome Destructor End");
        };
};

#endif
