#include <AbstractIntent.h>

const ActionResult ActionResult::VOID = {ActionRetultType::VOID, 0, IntentArgument::NO_ARG};

AbstractIntent::AbstractIntent(QueueHandle_t& mEventQueue): eventQueue(mEventQueue)
{
    ESP_LOGD(TAG_INTNT, "AbstractIntent constructor start");
	widgetGroup = lv_group_create();
	lv_indev_set_group(lv_get_keypad(), widgetGroup);
    ESP_LOGD(TAG_INTNT, "AbstractIntent constructor end");
}
