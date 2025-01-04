#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include <model/menu/MenuItem.h>
#include <model/AbstractModel.h>
#include <set/Set.h>

struct Menu : public AbstractModel
{
    private:
        Set<MenuItem> menuItems;

        /**
         * @returns either index of active menu item or -1 if no items are active
         */
        int16_t getActiveItemIdx();
        
    public:
        /**
         * Constructor with pre-defined items. Items passed my reference however afterwards copied to the object propety
         * 
         * @param pre-built set of menu items
        */
        Menu(DBox& box, Set<MenuItem>& itemsSet);

        /**
         * @returns reference to the set stored in the menu class
         */
        Set<MenuItem>& getItemsSet();

        /**
         * @returns pointer currrently active item
         */
        MenuItem* getActiveItem();

        /** 
         * Marks either next or previous menu item as active depending on a direction
         * removes active mark from previously acttive one
         * 
         * @param direction active item will be moved to the bottom if direction 
         */
        void moveActiveItem(uint8_t direction);
};

#endif