#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H

#include <AbstractIntent.h>
#include <IntentIdentifier.h>
#include <SleepControl.h>
#include <PinDefinitions.h>
#include <widget/WidgetStatus.h>
#include <model/status/ModelStatus.h>

struct StatusManager : public AbstractIntent
{

private:
    DBox boxStatus = DBox::stickTop(480, 32, 8, 1);

    WidgetStatus* widgetStatus = nullptr; 
    ModelStatus* modelStatus = nullptr;

    uint16_t frequency = 0;
    unsigned long lastExecution = 0;

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_STATUS;

    StatusManager(QueueHandle_t& mEventQueue);
    ~StatusManager()
    {
        delete modelStatus;
        delete widgetStatus;
    }

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;
};

#endif