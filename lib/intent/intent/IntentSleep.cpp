#include "IntentSleep.h"
#include "esp_log.h"
#include "LogTags.h"



IntentSleep::IntentSleep(QueueHandle_t& mEventQueue, SleepControl& sleepControl) :
    AbstractIntent(mEventQueue), sleepControl(sleepControl) {}

void IntentSleep::onStartUp(IntentArgument arg) {
    ESP_LOGD(TAG_INTNT, "IntentSleep::onStartUp");

    textBox    = new DBox{176, 368, 128, 64, 0, 1};
    widgetText = new WidgetText(widgetGroup, eventQueue);

    modelText = ModelText::newPtr();
    modelText->box   = *textBox;
    modelText->text  = "Zzzz...";
    modelText->align = LV_ALIGN_CENTER;

    widgetText->upgrade(*modelText);

    sleepPrepared = true;
}

void IntentSleep::onFrequncy() {
    ESP_LOGV(TAG_INTNT, "IntentSleep::onFrequncy");
    if (sleepPrepared && !imageDrawn) {
        
        lv_obj_t * img = lv_img_create(lv_scr_act());
        lv_img_set_src(img, "S:/.system/img/sleep_8bit.bmp");
        lv_obj_center(img);

        imageDrawn = true;
        return;
    }

    if (sleepPrepared && imageDrawn) {

        digitalWrite(PIN_LED, LOW);
        sleepControl.sleepNow();

        // Not needed but just in case restoring state
        sleepPrepared = false;
        imageDrawn = false;
    }
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
