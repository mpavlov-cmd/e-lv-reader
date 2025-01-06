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
            ESP_LOGD(TAG_WIDGT, "Text widget destructor end");
        }

    private: 
        lv_obj_t* label = nullptr;

        DBox createBox(ModelText& widgetData) override
        {
            return widgetData.box;
        }

        void initialize(ModelText& widgetData) override {

            label = lv_label_create(parent);
        }

        void print(ModelText& widgetData) override
        {
            lv_label_set_text(label, widgetData.text.c_str());
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        }
};

#endif