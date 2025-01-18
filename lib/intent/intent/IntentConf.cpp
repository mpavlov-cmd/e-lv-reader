#include "IntentConf.h"

IntentConf::IntentConf(QueueHandle_t &mEventQueue, ESP32Time &espTime) 
    : AbstractIntent(mEventQueue), espTime(espTime)
{
}

void IntentConf::onStartUp(IntentArgument arg)
{
    ESP_LOGD(TAG_INTNT, "IntentConf::onStartUp");

    widgetClockConf = new WidgetClockConf(widgetGroup, eventQueue);
    modelClock = new ModelClock(); 
    modelClock->box = boxClockConf; // TODO: Set remaining model values
    
    widgetClockConf->upgrade(*modelClock);
}

void IntentConf::onFrequncy()
{
    ESP_LOGV(TAG_INTNT, "IntentConf::onFrequncy");
}

void IntentConf::onExit()
{
    ESP_LOGV(TAG_INTNT, "IntentConf::onExit");
}

ActionResult IntentConf::onAction(ActionArgument arg)
{
    if (arg.code == LV_EVENT_CLICKED)
	{
        ESP_LOGD(TAG_INTNT, "IntentConf::onAction");
        uint8_t buttonUserData = (uint8_t)(uintptr_t)lv_obj_get_user_data(arg.target);

        // Zero for exit
        if (buttonUserData == 0) {
            return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
        }

    }
    return ActionResult::VOID;
}

uint8_t IntentConf::getId()
{
    return ID;
}
