#include "IntentHome.h"
#include "lvgl.h"

IntentHome::IntentHome(QueueHandle_t& mEventQueue, ESP32Time &espTime, FileManager &fm) :
AbstractIntent(mEventQueue), espTime(espTime), fileManager(fm) {}

void IntentHome::onStartUp(IntentArgument arg)
{
	// Group shoud be created after vl_init() so calling manually
	widgetGroup = lv_group_create();
	lv_indev_set_group(get_lv_keypad(), widgetGroup);

	// Draw image
	// lv_obj_t * img = lv_img_create(lv_scr_act());
    // lv_img_set_src(img, "S:/background/ninja_new_1_8bit.bmp");
    // lv_obj_center(img);

	// Define menu items
	Set<MenuItem> menuItems(10);

	// Main menu. For simplifity menu ID here MUST match intent ID
	menuItems.addItem(new MenuItem(INTENT_ID_FILE_SELECTOR, "Select Book", LV_SYMBOL_FILE, true));
	menuItems.addItem(new MenuItem(2, "Settings", LV_SYMBOL_SETTINGS));
	menuItems.addItem(new MenuItem(3, "Other", nullptr));
	menuItems.addItem(new MenuItem(INTENT_ID_SLEEP, "Sleep", LV_SYMBOL_EYE_CLOSE));

	// Define Box and Menu
	menuBox = new DBox{48, 432, 384, 256, 10, 5};
	menu = new Menu(*menuBox, "Main menu:", menuItems);

	// Create widgets
	menuParent = lv_obj_create(lv_scr_act());
	set_lv_active_object(menuParent);

	widgetMenu = new WidgetMenu(widgetGroup, menuParent, eventQueue);
	widgetMenu->upgrade(*menu);
}

void IntentHome::onFrequncy()
{
	ESP_LOGD(TAG_INTNT, "IntentHome::onFrequncy");
}

void IntentHome::onExit()
{
	ESP_LOGD(TAG_INTNT, "IntentHome::onExit");
	// Remove optimztion: set objet to null
	set_lv_active_object(nullptr);
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
	}

	return ActionResult::VOID;
}

uint8_t IntentHome::getId()
{
	return ID;
}
