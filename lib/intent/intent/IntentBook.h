#ifndef INTENTBOOK_H
#define INTENTBOOK_H

#include <AbstractIntent.h>
#include <IntentIdentifier.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <cache/DirectoryCache.h>
#include <text/TextIndex.h>

#include <widget/WidgetText.h>
#include <widget/WidgetBookStat.h>
#include <model/book/ModelBookStat.h>


struct IntentBook : public AbstractIntent
{
private:

    static constexpr const uint16_t PAGE_BUFFER_SIZE = 5120;

    FileManager& fileManager;
    TextIndex& textIndex;
    DirectoryCache &directoryCache;

    char bookPath[512];
    char bookIndexPath[1024];
    char bookPage[PAGE_BUFFER_SIZE];
    bool pageReady = false;
    bool pageShown = false;

    // Widgets
    DBox boxText = DBox::atCenter(480, 736, 8, 0);
    ModelText* modelText = nullptr;
    WidgetText* widgetText = nullptr;

    DBox boxBookStat = DBox::stickBottom(480, 32, 8, 0);
    ModelBookStat* modelBookStat = nullptr;
    WidgetBookStat* widgetBookStat = nullptr;

    // Task data
    TaskHandle_t bookLoadingHandle = NULL;

    void bookLoadingTask();

    // Static wrappers function for xTaskCreate
    static void bookLoadingEntry(void *param) {
        IntentBook *self = static_cast<IntentBook *>(param);
        self->bookLoadingTask(); // Call the private member function
    }

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_BOOK;

    IntentBook(QueueHandle_t& mEventQueue, FileManager &fileManager, TextIndex &textIndex, DirectoryCache &directoryCache);

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;

    ~IntentBook()
    {
        delete widgetText;
        delete modelText;
        delete widgetBookStat;
        delete modelBookStat;
    }
};

#endif