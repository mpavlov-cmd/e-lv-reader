#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include "AbstractWidget.h"
#include "model/menu/Menu.h"

class WidgetMenu : public AbstractWidget<Menu>
{

public:
    WidgetMenu(lv_group_t* wGroup, lv_obj_t* mParent, QueueHandle_t& mEventQueue) 
        : AbstractWidget(wGroup, mParent, mEventQueue) {}

    ~WidgetMenu() override {
        ESP_LOGD(TAG_WIDGT, "Menu widget destructor start");
        if (lv_obj_is_valid(list)) {
            lv_obj_del(list);
        }
        lv_style_reset(&style_font24);
        ESP_LOGD(TAG_WIDGT, "Menu widget destructor end");
    }

private:
    lv_obj_t * list = nullptr;
    lv_style_t style_font24;

    DBox createBox(Menu& widgetData) override
    {
        return widgetData.getBox();
    }

    void initialize(Menu& widgetData) override {

        list = lv_list_create(parent);

        lv_obj_set_size(list, box.width, box.height);
        lv_obj_center(list);

        // Create a style for menu
        lv_style_init(&style_font24);
        lv_style_set_text_font(&style_font24, &lv_font_montserrat_24); 
        
        // TODO: Figure out why padding is not applied from the parent box
        lv_style_set_pad_top(&style_font24, box.padding);    
        lv_style_set_pad_bottom(&style_font24, box.padding);

        lv_obj_add_style(list, &style_font24, LV_PART_MAIN); 
    }

    void print(Menu& widgetData) override
    {
        Set<MenuItem> &menuItems = widgetData.getItemsSet();
        lv_list_add_text(list, widgetData.getTitle());

        for (uint8_t i = 0; i < menuItems.size(); i++) {
            MenuItem* currentItem = menuItems.getItem(i);

            lv_obj_t * btn = lv_list_add_btn(list, currentItem->getIcon(), currentItem->getName());
            
            lv_obj_set_user_data(btn, currentItem);
            lv_group_add_obj(widgetGroup, btn);
            attachEventHandler(btn);

            if (i == 0) {
                lv_obj_add_state(btn, LV_STATE_CHECKED);
            }
        }
    }
};

#endif