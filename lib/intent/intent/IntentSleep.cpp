#include "IntentSleep.h"
#include "esp_log.h"
#include "LogTags.h"


IntentSleep::IntentSleep(QueueHandle_t& mEventQueue, SleepControl& sleepControl, ESP32Time& espTime) :
    AbstractIntent(mEventQueue), sleepControl(sleepControl), espTime(espTime) {}

void IntentSleep::onStartUp(IntentArgument arg) {
    ESP_LOGD(TAG_INTNT, "IntentSleep::onStartUp");

    // Display clock

    widgetClock = new WidgetClock(widgetGroup, eventQueue);

    modelClock = new ModelClock();
    modelClock->box = boxClock;
    ModelClock::updateWithEspTime(*modelClock, espTime);

    widgetClock->upgrade(*modelClock);

    // Display Text
    widgetText = new WidgetText(widgetGroup, eventQueue);

    modelText = ModelText::newPtr();
    modelText->box   = boxText;
    modelText->text  = "Sleeping like an angel";
    modelText->align = LV_ALIGN_CENTER;
    modelText->font  = &lv_font_montserrat_18; 

    widgetText->upgrade(*modelText);

    sleepPrepared = true;
}

void IntentSleep::onFrequncy() {
    ESP_LOGV(TAG_INTNT, "IntentSleep::onFrequncy");
    if (sleepPrepared && !imageDrawn) {
        
        // lv_obj_t * img = lv_img_create(lv_scr_act());
        // lv_img_set_src(img, "S:/.system/img/sleep_8bit.bmp");
        // lv_obj_center(img);

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
