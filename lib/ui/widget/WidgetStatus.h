#ifndef WIDGETSTATUS_H
#define WIDGETSTATUS_H

#include "AbstractWidget.h"
#include "model/status/ModelStatus.h"

class WidgetStatus : public AbstractWidget<ModelStatus>
{
    public:
        WidgetStatus(lv_group_t* wGroup, QueueHandle_t& mEventQueue) 
            : AbstractWidget(wGroup, mEventQueue) {}
        
        ~WidgetStatus() override {
            ESP_LOGD(TAG_WIDGT, "WidgetStatus destructor start");
            if (lv_obj_is_valid(labelBattInd)) {
                lv_obj_del(labelBattInd);
            }
             if (lv_obj_is_valid(labelPlugInd)) {
                lv_obj_del(labelPlugInd);
            }
            lv_style_reset(&widgetStyle);
            lv_style_reset(&battIndStyle);
            ESP_LOGD(TAG_WIDGT, "WidgetStatus destructor end");
        }
    
    private: 
        
        lv_obj_t* labelTimeInd = nullptr;
        lv_obj_t* labelBattInd = nullptr;
        lv_obj_t* labelPlugInd = nullptr;

        lv_style_t widgetStyle;
        lv_style_t battIndStyle;
        
        lv_coord_t battPercentWidth = 0;
        lv_coord_t clockWidth = 0;

        DBox createBox(ModelStatus& widgetData) override
        {
            return widgetData.box;
        }

        void initialize(ModelStatus& widgetData) override {
            // Set font to parent
            lv_style_set_text_font(&style, widgetData.lvFont);
            
            // Init style so lines have color and border radius is 0
            lv_style_init(&widgetStyle);

            lv_style_set_bg_color(&widgetStyle, lv_color_black());
            lv_style_set_bg_opa(&widgetStyle, LV_OPA_COVER); 

            lv_style_set_border_width(&widgetStyle, 0);
            lv_style_set_radius(&widgetStyle, 0);

            // Calculate width of battery percet based on the font
            String maxBtStr = "100%";
            battPercentWidth 
                = lv_txt_get_width(maxBtStr.c_str(), maxBtStr.length(), widgetData.lvFont, 1, LV_TEXT_FLAG_NONE);

            String maxClStr = "00:00";
            clockWidth
                = lv_txt_get_width(maxClStr.c_str(), maxClStr.length(), widgetData.lvFont, 1, LV_TEXT_FLAG_NONE);

            // Init style for battery indicator
            lv_style_init(&battIndStyle);
            lv_style_set_text_align(&battIndStyle, LV_TEXT_ALIGN_CENTER);

        }

        void print(ModelStatus& widgetData) override
        {
            // LV_SYMBOL_CHARGE

            // Add the text displaying currentPage/totalPages
            const char* battIndContent;
            String percentStrValue;

            if (!widgetData.plugged) {
                percentStrValue = String(widgetData.batteryLevel) + "%";
                battIndContent = percentStrValue.c_str();
            } else {
                if (widgetData.charging) {
                    battIndContent = LV_SYMBOL_CHARGE;
                } else {
                    battIndContent = LV_SYMBOL_BATTERY_FULL;
                }
            }

            labelBattInd = lv_label_create(parent);
            lv_obj_set_width(labelBattInd, battPercentWidth);
            lv_label_set_text(labelBattInd, battIndContent);
            lv_obj_add_style(labelBattInd, &battIndStyle, 0);

            lv_obj_align(labelBattInd, LV_ALIGN_RIGHT_MID, 0, 0);

            if (widgetData.plugged) {
                labelPlugInd = lv_label_create(parent);
                lv_label_set_text(labelPlugInd, LV_SYMBOL_USB);
                lv_obj_align_to(labelPlugInd, labelBattInd, LV_ALIGN_OUT_LEFT_MID, 0, 0);
            }

            labelTimeInd = lv_label_create(parent);
            lv_obj_set_width(labelTimeInd, clockWidth);
            lv_label_set_text(labelTimeInd, widgetData.time.c_str());
            lv_obj_align(labelTimeInd, LV_ALIGN_LEFT_MID, 0, 0);
        }
};

#endif