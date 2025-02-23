#ifndef INTENTFILESELECTOR_H
#define INTENTFILESELECTOR_H

#include <esp_log.h>
#include <LogTags.h>
#include <AbstractIntent.h>
#include <IntentIdentifier.h>
#include <FileManager.h>
#include <model/menu/Menu.h>
#include <widget/WidgetMenu.h>


struct IntentFileSelector : public AbstractIntent
{

private:
    static constexpr const uint16_t MAX_FILES = 1024;
    static constexpr const char* DIR = "<DIR>";

    FileManager& fileManager;

    Set<FileIndex> dirIndex; 
    String currentPath = "/";

    lv_obj_t* menuParent = nullptr;
    WidgetMenu* widgetMenu = nullptr;

    DBox menuBox{24, 48, 432, 352, 8, 1};
    Menu* menu = nullptr;

    void prepareAndRnderDir(const char* path);

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_FILE_SELECTOR;

    IntentFileSelector(QueueHandle_t& mEventQueue, FileManager& fm);

    ~IntentFileSelector()
    {
        ESP_LOGV(TAG_INTNT, "IntentFileSelector Destructor Start");
        delete menu;
        delete widgetMenu;
        if (lv_obj_is_valid(menuParent)) {
            lv_obj_del(menuParent);
        }
        ESP_LOGV(TAG_INTNT, "IntentFileSelector Destructor End");
    }

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;
};

#endif