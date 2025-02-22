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
        lv_style_reset(&styleList);
        lv_style_reset(&styleListButtonFocused);
        ESP_LOGD(TAG_WIDGT, "Menu widget destructor end");
    }

private:
    lv_obj_t * list = nullptr;
    lv_style_t styleList, styleListButtonFocused;

    DBox createBox(Menu& widgetData) override
    {
        return widgetData.getBox();
    }

    void initialize(Menu& widgetData) override {

        // Cancel the original padding and border to avoid clipping
        lv_style_set_border_width(&style, 0);
        lv_style_set_pad_all(&style, 0);

        list = lv_list_create(parent);

        lv_obj_set_width(list, box.width);
        lv_obj_set_height(list, box.height);
        lv_obj_align(list, LV_ALIGN_TOP_LEFT, 0, 0);

        // Create a style for menu
        lv_style_init(&styleList);
        lv_style_set_text_font(&styleList, &lv_font_montserrat_24);
        lv_style_set_pad_top(&styleList, box.padding); 
        lv_style_set_pad_bottom(&styleList, box.padding);

        lv_style_set_border_width(&styleList, box.border);
        lv_style_set_border_color(&styleList, lv_color_black());
        lv_style_set_border_side(&styleList, LV_BORDER_SIDE_FULL);
        lv_style_set_border_opa(&styleList, LV_OPA_COVER);

        lv_obj_add_style(list, &styleList, LV_PART_MAIN);
        
        // Create style for active item
        lv_style_init(&styleListButtonFocused);
        lv_style_set_text_color(&styleListButtonFocused, lv_color_white());
        lv_style_set_bg_color(&styleListButtonFocused, lv_color_black());
        lv_style_set_bg_opa(&styleListButtonFocused, LV_OPA_COVER);

        lv_obj_add_style(list, &styleScrollBar, LV_PART_SCROLLBAR);
    }

    void print(Menu& widgetData) override
    {
        // Clean any existing items
        lv_obj_clean(list);

        Set<MenuItem> &menuItems = widgetData.getItemsSet();
        lv_list_add_text(list, widgetData.getTitle());

        for (uint8_t i = 0; i < menuItems.size(); i++) {
            MenuItem* currentItem = menuItems.getItem(i);

            lv_obj_t * btn = lv_list_add_btn(list, currentItem->getIcon(), currentItem->getName());
            
            lv_obj_set_user_data(btn, currentItem);
            lv_group_add_obj(widgetGroup, btn);
            attachEventHandler(btn, LV_EVENT_PRESSED);

            // Use For LV_COLOR_DEPTH 1
            lv_obj_add_style(btn, &styleListButtonFocused, LV_STATE_FOCUS_KEY);

            if (i == 0) {
                lv_obj_add_state(btn, LV_STATE_CHECKED);
            }
        }
    }
};

#endif