#ifndef STATUSMANAGER_H
#define STATUSMANAGER_H

#include <AbstractIntent.h>
#include <IntentIdentifier.h>
#include <PinDefinitions.h>
#include <ESP32Time.h>
#include <PowerStatus.h>
#include <widget/WidgetStatus.h>
#include <model/status/ModelStatus.h>

struct StatusManager : public AbstractIntent
{

private:

    ESP32Time& espTime;
    PowerStatus& powerStatus;

    DBox boxStatus = DBox::stickTop(480, 32, 8, 1);

    WidgetStatus* widgetStatus = nullptr; 
    ModelStatus* modelStatus = nullptr;
    lv_obj_t* widgetParent = nullptr;

    uint16_t frequency = 0;
    unsigned long lastExecution = 0;

    char timeOutput[32];

    void fillModelStatus();

public:
    // Constant declaration
    static constexpr const uint8_t ID = INTENT_ID_STATUS;

    StatusManager(QueueHandle_t& mEventQueue, ESP32Time& mEspTime, PowerStatus& mPowerStatus);
    ~StatusManager()
    {
        delete modelStatus;
        delete widgetStatus;
        if (lv_obj_is_valid(widgetParent)) {
            lv_obj_del(widgetParent);
        } 
    }

    void onStartUp(IntentArgument arg) override;
    void onFrequncy() override;
    void onExit() override;

    ActionResult onAction(ActionArgument arg) override;
    uint8_t getId() override;
};

#endif