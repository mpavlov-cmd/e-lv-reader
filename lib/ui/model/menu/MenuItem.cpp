#include "MenuItem.h"

MenuItem::MenuItem(const uint16_t mId, const char *mName, const char* mValue, bool mIsActive = false):
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

bool MenuItem::getIsActive()
{
    return isActive;
}

void MenuItem::setIsActive(bool active)
{
    isActive = active;
}
