#include "Menu.h"

int16_t Menu::getActiveItemIdx()
{
    for (uint16_t i = 0; i < menuItems.size(); i++)
    {
        MenuItem *item = menuItems.getItem(i);
        if (item->getIsActive())
        {
            return i;
        }
    }

    return -1;
}

Menu::Menu(DBox& box, Set<MenuItem>& itemsSet) : AbstractModel(box), menuItems(itemsSet)
{
}

Set<MenuItem>& Menu::getItemsSet()
{
    return menuItems;
}

MenuItem* Menu::getActiveItem()
{
    uint16_t idx = getActiveItemIdx();
    if (idx == -1) {
        return nullptr;
    }

    return menuItems.getItem(idx);
}

void Menu::moveActiveItem(uint8_t direction)
{
    // Menu has at least one element
    uint16_t size = menuItems.size();
    if (size == 0) {
		return;
	}

    int16_t activeItemIdx = getActiveItemIdx();
    // If no active item activate the existing one
    if (activeItemIdx == -1) {
        menuItems.getItem(0)->setIsActive(true);
        return;
    }

    // Active item non-null
    // Deactivate current item
    menuItems.getItem(activeItemIdx)->setIsActive(false);

    // forward direction and currenyly active item is last
	if (direction && activeItemIdx == size - 1) {
		menuItems.getItem(0)->setIsActive(true);
		return;
	}

	// backward direction and currentkly active item is first
	if (!direction && activeItemIdx == 0) {
	    menuItems.getItem(size - 1)->setIsActive(true);
		return;
	}

	uint16_t newActiveIndex = direction ? activeItemIdx + 1 : activeItemIdx - 1;
	menuItems.getItem(newActiveIndex)->setIsActive(true);
}
