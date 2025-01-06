#include "MenuItem.h"

MenuItem::MenuItem(const uint16_t mId, const char *mName, const char* mValue, const char* mIcon, bool mIsActive = false):
    id(mId), isActive(mIsActive)
{
    // Allocate and copy name
    if (mName) {
        name = new char[strlen(mName) + 1];
        strcpy(const_cast<char*>(name), mName);
    } else {
        name = nullptr;
    }

    // Allocate and copy value
    if (mValue) {
        value = new char[strlen(mValue) + 1];
        strcpy(const_cast<char*>(value), mValue);
    } else {
        value = nullptr;
    }

    // Allocate and copy name
    if (mIcon) {
        icon = new char[strlen(mIcon) + 1];
        strcpy(const_cast<char*>(icon), mIcon);
    } else {
        icon = nullptr;
    }
}


uint16_t MenuItem::getId()
{
    return id;
}

const char* MenuItem::getName()
{
    return name;
}

const char *MenuItem::getValue()
{
    return value;
}

const char *MenuItem::getIcon()
{
    return icon;
}

bool MenuItem::getIsActive()
{
    return isActive;
}

void MenuItem::setIsActive(bool active)
{
    isActive = active;
}
