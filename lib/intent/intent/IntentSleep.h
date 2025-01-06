#ifndef INTENTSLEEP_H
#define INTENTSLEEP_H

#include <AbstractIntent.h>
#include <IntentIdentifier.h>
#include <SleepControl.h>
#include <PinDefinitions.h>
#include <widget/WidgetText.h>
#include <model/text/ModelText.h>

struct IntentSleep : public AbstractIntent
{

private: 
    SleepControl& sleepControl;
    bool sleepPrepared = false;
    bool imageDrawn    = false;

    WidgetText* widgetText = nullptr;
    ModelText* modelText = nullptr;
    DBox* textBox = nullptr;

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_SLEEP;

    IntentSleep(QueueHandle_t& mEventQueue, SleepControl &sleepControl);
    ~IntentSleep()
    {
        delete widgetText;
        delete modelText;
        delete textBox;
    }

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;
};

#endif