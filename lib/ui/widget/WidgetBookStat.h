#ifndef WIDGETBOOKSTAT_H
#define WIDGETBOOKSTAT_H

#include "AbstractWidget.h"
#include "model/book/ModelBookStat.h"

class WidgetBookStat : public AbstractWidget<ModelBookStat>
{
    public:
        WidgetBookStat(lv_group_t* wGroup, QueueHandle_t& mEventQueue) 
            : AbstractWidget(wGroup, mEventQueue) {}
        
        ~WidgetBookStat() override {
            ESP_LOGD(TAG_WIDGT, "WidgetBookStat destructor start");
            if (lv_obj_is_valid(line)) {
                lv_obj_del(line);
            }
            if (lv_obj_is_valid(tick)) {
                lv_obj_del(tick);
            }
            if (lv_obj_is_valid(label))
            {
                lv_obj_del(label);
            }
            lv_style_reset(&widgetStyle);
            ESP_LOGD(TAG_WIDGT, "WidgetBookStat destructor end");
        }
    
    private: 

        lv_obj_t* line  = nullptr;
        lv_obj_t* tick  = nullptr;
        lv_obj_t* label = nullptr;

        lv_style_t widgetStyle;

        DBox createBox(ModelBookStat& widgetData) override
        {
            return widgetData.box;
        }

        void initialize(ModelBookStat& widgetData) override {
            // Set font to parent
            lv_style_set_text_font(&style, widgetData.lvFont);
            
            // Init style so lines have color and border radius is 0
            lv_style_init(&widgetStyle);

            lv_style_set_bg_color(&widgetStyle, lv_color_black());
            lv_style_set_bg_opa(&widgetStyle, LV_OPA_COVER); 

            lv_style_set_border_width(&widgetStyle, 0);
            lv_style_set_radius(&widgetStyle, 0);

            line = lv_obj_create(parent);
            lv_obj_align(line, LV_ALIGN_LEFT_MID, 0, 0);
            lv_obj_add_style(line, &widgetStyle, 0);

            tick = lv_obj_create(parent);
            lv_obj_add_style(tick, &widgetStyle, 0);

            label = lv_label_create(parent);
        }

        void print(ModelBookStat& widgetData) override
        {
            // Calculate text dimensions
            char labelText[16];
            snprintf(labelText, sizeof(labelText), "%u/%u", widgetData.currentPage, widgetData.totalPages);
            String labelTextStr = String(labelText);

            // Find bar width
            lv_coord_t labelCoord = lv_txt_get_width(labelText, labelTextStr.length(), widgetData.lvFont, 1, LV_TEXT_FLAG_NONE);
            uint16_t barWidth = box.width - box.padding * 2 - labelCoord - 16;

            lv_obj_set_size(line, barWidth, 2);

             // Calculate tick position based on the current page
            uint16_t tickWidth  = 2; 
            uint16_t tickHeight = 8;

            // Avoid division by 0 and 0 as current page
            uint16_t tickPositionX;
            if (widgetData.totalPages <= 1 || widgetData.currentPage == 0)
            {
                tickPositionX = 0; 
            }
            else
            {
                tickPositionX = ((widgetData.currentPage - 1) * barWidth) / (widgetData.totalPages - 1);
            }
            uint16_t tickPositionY = (box.height - box.padding * 2 - box.border * 2) / 2 - tickHeight / 2;

            lv_obj_set_size(tick, tickWidth, tickHeight);
            lv_obj_set_pos(tick, tickPositionX, tickPositionY);

            // Add the text displaying currentPage/totalPages
            lv_label_set_text(label, labelText);
            lv_obj_align(label, LV_ALIGN_RIGHT_MID, -5, 0);
        }
};

#endif