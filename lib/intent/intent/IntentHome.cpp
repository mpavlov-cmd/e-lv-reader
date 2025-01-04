#include "IntentHome.h"
#include "lvgl.h"

IntentHome::IntentHome(ESP32Time &espTime, FileManager &fm) : espTime(espTime), fileManager(fm) {}

void IntentHome::onStartUp(IntentArgument arg)
{
	// Draw image
	lv_obj_t * img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, "S:/background/ninja_new_1_8bit.bmp");
    lv_obj_center(img);

	// Define menu items
	Set<MenuItem> menuItems(10);

	// Main menu. For simplifity menu ID here MUST match intent ID
	menuItems.addItem(new MenuItem(INTENT_ID_FILE_SELECTOR, "Select Book", true));
	menuItems.addItem(new MenuItem(2, "Settings"));
	menuItems.addItem(new MenuItem(3, "Other"));
	menuItems.addItem(new MenuItem(4, "One more item for fun"));
	menuItems.addItem(new MenuItem(INTENT_ID_SLEEP, "Sleep"));

	// Define Box and Menu
	// TODO: Avoid allocat
	menuBox = new DBox{48, 584, 384, 160, 0, 0};
	menu = new Menu(*menuBox, menuItems);

	// Create widgets
	widgetMenu = new WidgetMenu();
	widgetMenu->upgrade(*menu);
}

void IntentHome::onFrequncy()
{
	ESP_LOGD(TAG_INTNT, "IntentHome::onFrequncy");
}

void IntentHome::onExit()
{
	ESP_LOGD(TAG_INTNT, "IntentHome::onExit");
}
ActionResult IntentHome::onAction(ActionArgument arg)
{
	// Handle up or down
	// if (handleMenuNavigation(*menu, *widgetMenu, arg.actionBit)) {
	// 	return ActionResult::VOID;
	// }

	// // Anyting else
	// if (arg.actionBit =! B00001000)
	// {
	// 	return ActionResult::VOID;
	// }

	// Serial.println("--- Home Intent Enter Clicked ---");

	// // Validate active item and menu id value
	// if (menu->getActiveItem() == nullptr) {
	// 	return ActionResult::VOID;
	// }

	// uint16_t menuItemId = menu->getActiveItem()->getId();
	// if (menuItemId > UINT8_MAX) {
	// 	return ActionResult::VOID;
	// }

	// // From main menu go to FS root
	// IntentArgument fileSelectorFsRoot("/");

	// switch (menuItemId)
	// {
	// case INTENT_ID_SLEEP:
	// 	return {ActionRetultType::CHANGE_INTENT, INTENT_ID_SLEEP, IntentArgument::NO_ARG};
	// case INTENT_ID_FILE_SELECTOR:
	// 	return {ActionRetultType::CHANGE_INTENT, INTENT_ID_FILE_SELECTOR, fileSelectorFsRoot};
	// default:
	// 	return ActionResult::VOID;
	// }	
	
	return ActionResult::VOID;
}

uint8_t IntentHome::getId()
{
	return ID;
}