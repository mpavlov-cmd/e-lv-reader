#include "StatusManager.h"
#include "TimeUtils.h"

void StatusManager::fillModelStatus()
{
    if (modelStatus == nullptr) {
        modelStatus = new ModelStatus();
        modelStatus->box = boxStatus;
        modelStatus->lvFont =  &lv_font_montserrat_18;
    }

    // Get and format time
    memset(modelStatus->time, '\0', sizeof(modelStatus->time));
    formatTime(espTime.getHour(true), espTime.getMinute(), espTime.getSecond(), "HH:MM", modelStatus->time);

    PowerMetrics powerMetrics = powerStatus.measure();
    modelStatus->plugged  = powerMetrics.isConnected;
    modelStatus->charging = powerMetrics.chargingStatus == CHARGING;
    modelStatus->batteryLevel = powerMetrics.battLevelPercent;
    strncpy(modelStatus->extra, String(powerMetrics.battVoltageMillivolts).c_str(), sizeof(modelStatus->extra));
}

StatusManager::StatusManager(QueueHandle_t &mEventQueue, ESP32Time &mEspTime, PowerStatus &mPowerStatus) 
    : AbstractIntent(mEventQueue), espTime(mEspTime), powerStatus(mPowerStatus) {}

void StatusManager::onStartUp(IntentArgument arg)
{
    ESP_LOGD(TAG_STAT, "StatusManager::onStartUp");

    // Init timer values
    frequency = arg.intValue;
    lastExecution = millis();

    widgetParent = lv_obj_create(lv_scr_act());
    fillModelStatus();
    widgetStatus = new WidgetStatus(widgetGroup, widgetParent, eventQueue);

    widgetStatus->upgrade(*modelStatus);
}

void StatusManager::onFrequncy()
{
    unsigned long time = millis();
    if (time - lastExecution > frequency) {
        // Run task on frequency
        ESP_LOGD(TAG_STAT, "StatusManager::onFrequncy");
        fillModelStatus();
        widgetStatus->upgrade(*modelStatus);
        
        // Update last execution
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
