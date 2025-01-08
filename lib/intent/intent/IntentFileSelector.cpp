#include "IntentFileSelector.h"

void IntentFileSelector::prepareAndRnderDir(const char *path)
{
    // Save current path 
    currentPath = path;
    const char* levelUpPath = getParentDir(path);

    // Init menu items
    Set<MenuItem> menuItems(MAX_FILES);

    // Add top level item
    String dirString = String(DIR) + "..";
    MenuItem* parentLevelitem = new MenuItem(0, dirString.c_str(), levelUpPath, nullptr, false);
    menuItems.addItem(parentLevelitem);

    // Build dir index
    dirIndex.clear();
    bool indexed = fileManager.indexDirectory(path, DirIndexConf::NO_HIDDEN, dirIndex);

    for (uint16_t i = 0; i < dirIndex.size(); i++) {

        FileIndex* fi = dirIndex.getItem(i);
        bool isActive = i==0;
        uint16_t id = i + 1;

        MenuItem* menuItem = nullptr;
        if (fi->getIsDirectry()) {
            size_t newLength = strlen(DIR) + strlen(fi->getName()) + 1; // +1 for null terminator
            char* newValue = new char[newLength];

            // Combine the strings
            strcpy(newValue, DIR);
            strcat(newValue, fi->getName());

            menuItem = new MenuItem(id, newValue, fi->getPath(), nullptr, isActive);
            delete newValue;
        } else {
            menuItem = new MenuItem(id, fi->getName(), fi->getPath(), nullptr, isActive);
        }

        // Add item to the menu
        menuItems.addItem(menuItem);
    }
  
    menu = new Menu(menuBox, "Select file:", menuItems);

    // Create widgets
	menuParent = lv_obj_create(lv_scr_act());
	set_lv_active_object(menuParent);

    widgetMenu = new WidgetMenu(widgetGroup, menuParent, eventQueue);
    widgetMenu->upgrade(*menu);
}

IntentFileSelector::IntentFileSelector(QueueHandle_t& mEventQueue, FileManager &fm)
    : AbstractIntent(mEventQueue), fileManager(fm), dirIndex(MAX_FILES) 
{
}

void IntentFileSelector::onStartUp(IntentArgument arg)
{
    // TODO: Duplicate code move to abstract intent
    // Group shoud be created after vl_init() so calling manually
	widgetGroup = lv_group_create();
	lv_indev_set_group(get_lv_keypad(), widgetGroup);

    ESP_LOGV(TAG_INTNT, "Intent FileSelector On Startup Called with argument: %s", arg.strValue);
    const char* rootPath = arg.strValue == nullptr ? "/" : arg.strValue;

    prepareAndRnderDir(rootPath);
}

void IntentFileSelector::onFrequncy()
{
    ESP_LOGV(TAG_INTNT, "IntentFileSelector::onFrequncy");
}

void IntentFileSelector::onExit()
{
    ESP_LOGD(TAG_INTNT, "IntentFileSelector::onExit");
}

ActionResult IntentFileSelector::onAction(ActionArgument arg)
{
    // const char* newFilePath = activeItem->getValue();
    if (arg.code == LV_EVENT_CLICKED)
    {
        MenuItem* clicked = static_cast<MenuItem*>(lv_obj_get_user_data(arg.target));
		ESP_LOGD(TAG_INTNT, "Got user data from event. ID: %i, Name: %s", clicked->getId(), clicked->getName());

        const char* newFilePath = clicked->getValue();
        ESP_LOGD(TAG_INTNT, "Selected file: %s", newFilePath);

        File selectedFile = fileManager.openFile(newFilePath, FILE_READ);
        bool isDirectory = selectedFile.isDirectory();

        // Make sure the value is copied, otherwise after file is closed pointer would be deleted
        static char storedPacthCa[256];
        strncpy(storedPacthCa, selectedFile.path(), sizeof(storedPacthCa) - 1);
        storedPacthCa[sizeof(storedPacthCa) - 1] = '\0';

        selectedFile.close();

        // Get char array for the stored path
        if (!isDirectory)
        {
            const char *fileExt = getFileExtension(storedPacthCa);

            // Choose new intent based on the extention
            if (strcmp("txt", fileExt) == 0)
            {
                IntentArgument bookArg(storedPacthCa);
                return {ActionRetultType::CHANGE_INTENT, INTENT_ID_BOOK, bookArg};
            }

            return ActionResult::VOID;
        }

        if (currentPath == "/" && strcmp("/", storedPacthCa) == 0)
        {
            return {ActionRetultType::CHANGE_INTENT, INTENT_ID_HOME, IntentArgument::NO_ARG};
        }

        // File is directory cleanup and restart
        delete menu;
        delete widgetMenu;
        if (lv_obj_is_valid(menuParent)) {
            lv_obj_del(menuParent);
        }

        prepareAndRnderDir(storedPacthCa);
        return ActionResult::VOID;
    }

    return ActionResult::VOID;
}

uint8_t IntentFileSelector::getId()
{
    return ID;
}
