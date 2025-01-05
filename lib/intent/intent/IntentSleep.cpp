#include "IntentSleep.h"
#include "esp_log.h"
#include "LogTags.h"

IntentSleep::IntentSleep(QueueHandle_t& mEventQueue, SleepControl& sleepControl) :
    AbstractIntent(mEventQueue), sleepControl(sleepControl) {}

void IntentSleep::onStartUp(IntentArgument arg) {
    ESP_LOGD(TAG_INTNT, "IntentSleep::onStartUp");

    // widgetImage = new WidgetImage(display, imageDrawer, fileManager);
    // imgModel = {"/.system/img/sleep.bmp", CENTER_CENTER};
    // widgetImage->upgrade(imgModel);

	digitalWrite(PIN_LED, LOW);
    sleepControl.sleepNow();
}

void IntentSleep::onFrequncy() {
    ESP_LOGD(TAG_INTNT, "IntentSleep::onFrequncy");
}

void IntentSleep::onExit()
{
}

ActionResult IntentSleep::onAction(ActionArgument arg)
{
    return ActionResult::VOID;
}

uint8_t IntentSleep::getId()
{
    return ID;
}
