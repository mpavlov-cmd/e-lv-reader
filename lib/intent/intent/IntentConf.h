#ifndef INTENTCONF_H
#define INTENTCONF_H

#include <AbstractIntent.h>
#include <IntentIdentifier.h>
#include <PinDefinitions.h>

#include <SleepControl.h>
#include <ESP32Time.h>

#include <widget/WidgetClockConf.h>
#include <model/clock/ModelClock.h>

struct IntentConf : public AbstractIntent
{

private: 
    ESP32Time& espTime;

    // Define clock consf widget and model
    DBox boxClockConf = DBox::atCenter(464, 240, 8, 1);
    WidgetClockConf* widgetClockConf = nullptr;
    ModelClock* modelClock = nullptr;

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_CONF;

    IntentConf(QueueHandle_t& mEventQueue, ESP32Time& espTime);
    ~IntentConf()
    {
        delete widgetClockConf;
        delete modelClock;
    }

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;
};

#endif