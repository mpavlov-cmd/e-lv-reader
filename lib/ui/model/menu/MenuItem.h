#ifndef MENUITEM_H
#define MENUITEM_H

#include <Arduino.h>

struct MenuItem
{
    private:
        const uint16_t id;
        const char* name;
        const char* value;
        const char* icon;
        bool isActive = false;

    public:
        // Primary 
        MenuItem(const uint16_t mId, const char* mName, const char* mValue, const char* mIcon, bool mIsActive);

        // Secondary
        MenuItem(const uint16_t mId, const char* mName, const char* mIcon, bool mIsActive):
            MenuItem(mId, mName, nullptr, mIcon, mIsActive) {};
        MenuItem(const uint16_t mId, const char* mName, const char* mIcon): 
            MenuItem(mId, mName, nullptr, mIcon, false) {};

        // Copy constructor
        MenuItem(const MenuItem& other) : MenuItem(other.id, other.name, other.value, other.icon, other.isActive) {}; 

        ~MenuItem() {
            delete name;
            delete value;
            delete icon;
        };

        uint16_t getId();
        const char* getName();
        const char* getValue();
        const char* getIcon();
        bool getIsActive();
        void setIsActive(bool active);
};


#endif