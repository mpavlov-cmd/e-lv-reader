#ifndef MENUITEM_H
#define MENUITEM_H

#include <Arduino.h>

struct MenuItem
{
    private:
        const uint16_t id;
        const char* name;
        const char* value;
        bool isActive = false;

    public:
        // Primary 
        MenuItem(const uint16_t mId, const char* mName, const char* value, bool mIsActive);

        // Secondary
        MenuItem(const uint16_t mId, const char* mName, bool mIsActive): MenuItem(mId, mName, nullptr, mIsActive) {};
        MenuItem(const uint16_t mId, const char* mName): MenuItem(mId, mName, nullptr, false) {};
        MenuItem(const MenuItem& other) : MenuItem(other.id, other.name, other.value, other.isActive) {}; 

        ~MenuItem() {
            delete name;
            delete value;
        };

        uint16_t getId();
        const char* getName();
        const char* getValue();
        bool getIsActive();
        void setIsActive(bool active);
};


#endif