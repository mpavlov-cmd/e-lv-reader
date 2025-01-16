#ifndef INTENTSLEEP_H
#define INTENTSLEEP_H

#include <AbstractIntent.h>
#include <IntentIdentifier.h>
#include <PinDefinitions.h>

#include <SleepControl.h>
#include <ESP32Time.h>

#include <widget/WidgetClock.h>
#include <model/clock/ModelClock.h>
#include <widget/WidgetText.h>
#include <model/text/ModelText.h>

struct IntentSleep : public AbstractIntent
{

private: 
    SleepControl& sleepControl;
    ESP32Time& espTime;

    bool sleepPrepared = false;
    bool imageDrawn    = false;

    DBox boxText  = DBox::stickBottom(256, 64, 4, 0);
    DBox boxClock = DBox::atCenter(256, 128, 8, 0);

    WidgetText* widgetText = nullptr;
    ModelText* modelText = nullptr;

    WidgetClock* widgetClock = nullptr;
    ModelClock* modelClock = nullptr;

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_SLEEP;

    IntentSleep(QueueHandle_t& mEventQueue, SleepControl &sleepControl, ESP32Time& espTime);
    ~IntentSleep()
    {
        delete widgetText;
        delete modelText;
        delete widgetClock;
        delete modelClock;
    }

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;
};

#endif