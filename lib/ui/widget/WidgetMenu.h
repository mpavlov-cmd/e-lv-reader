#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include "AbstractWidget.h"
#include "model/menu/Menu.h"

class WidgetMenu : public AbstractWidget<Menu>
{

public:
    WidgetMenu(lv_group_t* wGroup) : AbstractWidget(wGroup) {}
    ~WidgetMenu() override {
        ESP_LOGD(TAG_WIDGT, "Menu widget destructor start");
        if (list != nullptr) {
            lv_obj_del(list);
            lv_style_reset(&style_font24);
        }
        ESP_LOGD(TAG_WIDGT, "Menu widget destructor start");
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
        // TODO: Take padding into consideratio
        lv_obj_set_size(list, box.width, box.height);
        lv_obj_center(list);

        // Create a style for menu
        lv_style_init(&style_font24);
        lv_style_set_text_font(&style_font24, &lv_font_montserrat_24); 

        lv_obj_add_style(list, &style_font24, LV_PART_MAIN); 
        lv_obj_add_style(list, &style_font24, LV_PART_ITEMS);
    }

    void beforePrint(Menu& widgetData) override {

    }

    void print(Menu& widgetData) override
    {
        Set<MenuItem> &menuItems = widgetData.getItemsSet();
        lv_list_add_text(list, widgetData.getTitle());

        for (uint8_t i = 0; i < menuItems.size(); i++) {
            MenuItem* currentItem = menuItems.getItem(i);

            lv_obj_t * btn = lv_list_add_btn(list, LV_SYMBOL_FILE, currentItem->getName());
            lv_group_add_obj(widgetGroup, btn);

            // TODO: add event handler
            // btn->user_data = 
            // lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_CLICKED, NULL);
            if (i == 0) {
                lv_obj_add_state(btn, LV_STATE_CHECKED);
            }
        }
    }

    void afterPrint(Menu& widgetData) override {
       
    }
};

#endif