#ifndef INTENTSLEEP_H
#define INTENTSLEEP_H

#include <AbstractIntent.h>
#include <IntentIdentifier.h>
#include <SleepControl.h>
#include <PinDefinitions.h>
#include <model/DPosition.h>

struct IntentSleep : public AbstractIntent
{

private: 
    SleepControl& sleepControl;

    // WidgetImage* widgetImage = nullptr;
    // ImageModel imgModel;

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_SLEEP;

    IntentSleep(QueueHandle_t& mEventQueue, SleepControl &sleepControl);
    ~IntentSleep()
    {
        //delete widgetImage;
    }

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;
};

#endif