#ifndef WIDGETCLOCK_H
#define WIDGETCLOCK_H

#include "AbstractWidget.h"
#include "model/clock/ModelClock.h"
#include "TimeUtils.h"

class WidgetClock : public AbstractWidget<ModelClock>
{
    public:
        WidgetClock(lv_group_t* wGroup, QueueHandle_t& mEventQueue) 
            : AbstractWidget(wGroup, mEventQueue) {}
        
        ~WidgetClock() override {
            ESP_LOGD(TAG_WIDGT, "WidgetClock destructor start");
            if (lv_obj_is_valid(labelTime)) {
                lv_obj_del(labelTime);
            }
            if (lv_obj_is_valid(labelDate)) {
                lv_obj_del(labelDate);
            }
             if (lv_obj_is_valid(parentDateTime)) {
                lv_obj_del(parentDateTime);
            }
            lv_style_reset(&styleTime);
            lv_style_reset(&styleDate);
            lv_style_reset(&styleParentDateTime);
            ESP_LOGD(TAG_WIDGT, "WidgetClock destructor end");
        }
    
    private: 

        lv_obj_t* parentDateTime = nullptr;

        lv_obj_t* labelTime = nullptr;
        lv_obj_t* labelDate = nullptr;
        lv_style_t styleTime;
        lv_style_t styleDate;
        lv_style_t styleParentDateTime;

        DBox createBox(ModelClock& widgetData) override
        {
            return widgetData.box;
        }

        void initialize(ModelClock& widgetData) override {

            String maxClStr = "00:00";
            lv_coord_t clockWidth
                = lv_txt_get_width(maxClStr.c_str(), maxClStr.length(), &lv_font_montserrat_48, 1, LV_TEXT_FLAG_NONE);

            lv_style_init(&styleParentDateTime);
            lv_style_set_border_width(&styleParentDateTime, 0);
            lv_style_set_border_color(&styleParentDateTime, lv_color_black());
            lv_style_set_border_opa(&styleParentDateTime, LV_OPA_COVER);
            lv_style_set_pad_all(&styleParentDateTime, 0);   

            parentDateTime = lv_obj_create(parent);
            lv_obj_align(parentDateTime, LV_ALIGN_CENTER, 0, 0);
            lv_obj_set_width(parentDateTime, clockWidth);
            lv_obj_set_height(parentDateTime, 88);
            lv_obj_add_style(parentDateTime, &styleParentDateTime, LV_PART_MAIN);
                        
            // Time
            labelTime = lv_label_create(parentDateTime);
            lv_obj_align(labelTime, LV_ALIGN_TOP_LEFT, 0, 0);

            lv_style_init(&styleTime);
            lv_style_set_text_font(&styleTime, &lv_font_montserrat_48);

            lv_obj_add_style(labelTime, &styleTime, LV_PART_MAIN); 

            // Date
            labelDate = lv_label_create(parentDateTime);
            lv_obj_align_to(labelDate, labelTime, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

            lv_style_init(&styleDate);
            lv_style_set_text_font(&styleDate, &lv_font_montserrat_24);

            lv_obj_add_style(labelDate, &styleDate, LV_PART_MAIN); 
        }

        void print(ModelClock& widgetData) override
        {
            char timeText[6];
            formatTime(widgetData.hour, widgetData.min, widgetData.sec, "HH:MM", timeText);
            lv_label_set_text(labelTime, timeText);

            String dateText = String(widgetData.day) + "/" + String(widgetData.month) + "/" + String(widgetData.year);
            lv_label_set_text(labelDate, dateText.c_str());
        }
};

#endif
