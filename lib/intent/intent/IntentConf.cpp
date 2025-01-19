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
    modelClock->box = boxClockConf;
    ModelClock::updateWithEspTime(*modelClock, espTime);
    
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
        WidgetClockConf::Action* action = (WidgetClockConf::Action*)lv_obj_get_user_data(arg.target);

        // Save time
        if (action->id == 1) {
            ModelClock* model = action->data;

            // Log seleted value
            ESP_LOGD(
                TAG_INTNT, "%i:%i:%i %i/%i/%i", 
                model->hour, model->min, model->sec, model->day, model->month, model->year
            );

            espTime.setTime(model->sec, model->min, model->hour, model->day, model->month, model->year, 0);

            // Temporary return home after timer is set
            return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
        }

        // Zero for exit
        if (action->id == 0) {
            return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
        }

    }
    return ActionResult::VOID;
}

uint8_t IntentConf::getId()
{
    return ID;
}
