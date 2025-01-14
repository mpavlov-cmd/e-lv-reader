#include "StatusManager.h"

StatusManager::StatusManager(QueueHandle_t &mEventQueue) : AbstractIntent(mEventQueue)
{
}

void StatusManager::onStartUp(IntentArgument arg)
{
    ESP_LOGD(TAG_STAT, "StatusManager::onStartUp");

    // Init timer values
    frequency = arg.intValue;
    lastExecution = millis();

    widgetParent = lv_obj_create(lv_scr_act());
    modelStatus = new ModelStatus{boxStatus, false, false, 3, "00:00", "", &lv_font_montserrat_18};
    widgetStatus = new WidgetStatus(widgetGroup, widgetParent, eventQueue);
    widgetStatus->upgrade(*modelStatus);

    // Create and run the timer
    // timer = lv_timer_create(lv_timer, arg.intValue, this);
}

void StatusManager::onFrequncy()
{
    unsigned long time = millis();
    if (time - lastExecution > frequency) {
        ESP_LOGD(TAG_STAT, "StatusManager::onFrequncy");
        strncpy(modelStatus->extra, String(time).c_str(), sizeof(modelStatus->extra));
        widgetStatus->upgrade(*modelStatus);
        lastExecution = time;
    }   
}

void StatusManager::onExit()
{
    ESP_LOGD(TAG_STAT, "StatusManager::onExit");
}

ActionResult StatusManager::onAction(ActionArgument arg)
{
    // Invalidate object to force reresh widget e.g., during change intent
    lv_obj_invalidate(widgetParent);
    return ActionResult::VOID;
}

uint8_t StatusManager::getId()
{
    return ID;
}
