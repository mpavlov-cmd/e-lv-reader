#ifndef WIDGETTEXT_H
#define WIDGETTEXT_H

#include "AbstractWidget.h"
#include "model/text/ModelText.h"

class WidgetText : public AbstractWidget<ModelText>
{
    public:
        WidgetText(lv_group_t* wGroup, QueueHandle_t& mEventQueue) 
            : AbstractWidget(wGroup, mEventQueue) {}
        
        ~WidgetText() override {
            ESP_LOGD(TAG_WIDGT, "Text widget destructor start");
            if (lv_obj_is_valid(label)) {
                lv_obj_del(label);
            }
            if (lv_obj_is_valid(dummy)) {
                lv_obj_del(dummy);
            }
            ESP_LOGD(TAG_WIDGT, "Text widget destructor end");
        }

    private: 
        lv_obj_t* label = nullptr;
        // Dummy object to addinto group so I can still read prev and next input
        lv_obj_t* dummy = nullptr;

        DBox createBox(ModelText& widgetData) override
        {
            return widgetData.box;
        }

        void initialize(ModelText& widgetData) override {

            label = lv_label_create(parent);
            lv_style_set_text_font(&style, widgetData.font);

            // In case input handling is required for widget e.g., display more text on action
            if (widgetData.hasAction)
            {
                // Add to group and attach event handler
                lv_group_add_obj(widgetGroup, label);
                attachEventHandler(label);

                // Dummy objet to handle nav
                dummy = lv_label_create(parent);
                lv_label_set_text(dummy, "");
                // lv_obj_add_flag(dummy, LV_OBJ_FLAG_HIDDEN);
                lv_group_add_obj(widgetGroup, dummy);
                attachEventHandler(dummy);
            }
        }

        void print(ModelText& widgetData) override
        {
            lv_label_set_text(label, widgetData.text.c_str());
            lv_obj_align(label, widgetData.align, 0, 0);
        }
};

#endif