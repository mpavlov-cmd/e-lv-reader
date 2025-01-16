#include "IntentHome.h"
#include "lvgl.h"

void IntentHome::updateTime()
{
	ModelClock::updateWithEspTime(*modelClock, espTime);
}

IntentHome::IntentHome(QueueHandle_t &mEventQueue, ESP32Time &espTime, FileManager &fm) : AbstractIntent(mEventQueue), espTime(espTime), fileManager(fm) {}

void IntentHome::onStartUp(IntentArgument arg)
{
	// Store minute for further updates
	lastMinute = espTime.getMinute();

	// Define menu items
	Set<MenuItem> menuItems(10);

	// Main menu. For simplifity menu ID here MUST match intent ID
	menuItems.addItem(new MenuItem(INTENT_ID_FILE_SELECTOR, "Select Book", LV_SYMBOL_FILE, true));
	menuItems.addItem(new MenuItem(2, "Settings", LV_SYMBOL_SETTINGS));
	menuItems.addItem(new MenuItem(3, "Other", nullptr));
	menuItems.addItem(new MenuItem(INTENT_ID_SLEEP, "Sleep", LV_SYMBOL_EYE_CLOSE));

	// Define Menu
	menu = new Menu(boxMenu, "Main menu:", menuItems);

	// Create widgets
	menuParent = lv_obj_create(lv_scr_act());
	widgetMenu = new WidgetMenu(widgetGroup, menuParent, eventQueue);
	widgetMenu->upgrade(*menu);

	boxClock.y = boxClock.y - 160;
	modelClock = new ModelClock();
	modelClock->box = boxClock;
	updateTime();

	widgetClock = new WidgetClock(widgetGroup, eventQueue);
	widgetClock->upgrade(*modelClock);
}

void IntentHome::onFrequncy()
{	
	int currentMinute = espTime.getMinute();
	if (lastMinute == currentMinute) {
		return;
	}
	
	ESP_LOGV(TAG_INTNT, "IntentHome::onFrequncy");
	updateTime();
	widgetClock->upgrade(*modelClock);

	// Storecurrent minute as procesed
	lastMinute = currentMinute;
}

void IntentHome::onExit()
{
	ESP_LOGD(TAG_INTNT, "IntentHome::onExit");
}

ActionResult IntentHome::onAction(ActionArgument arg)
{
	// ESP_LOGD(TAG_INTNT, "IntentHome::onAction EVENT  %i", arg.code);
	// Ony handle click events
	if (arg.code == LV_EVENT_CLICKED)
	{
		MenuItem* clicked = static_cast<MenuItem*>(lv_obj_get_user_data(arg.target));
		ESP_LOGD(TAG_INTNT, "Got user data from event. ID: %i, Name: %s", clicked->getId(), clicked->getName());
		
		if (clicked->getId() == INTENT_ID_SLEEP) {
			return {ActionRetultType::CHANGE_INTENT, INTENT_ID_SLEEP, IntentArgument::NO_ARG};
		}

		if (clicked->getId() == INTENT_ID_FILE_SELECTOR) {
			// From main menu go to FS root
			IntentArgument fsRoot("/");
			return {ActionRetultType::CHANGE_INTENT, INTENT_ID_FILE_SELECTOR, fsRoot};
		}
	}

	return ActionResult::VOID;
}

uint8_t IntentHome::getId()
{
	return ID;
}
