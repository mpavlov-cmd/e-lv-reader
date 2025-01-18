#ifndef WIDGETCLOCKCONF_H
#define WIDGETCLOCKCONF_H

#include "AbstractWidget.h"
#include "model/clock/ModelClock.h"
#include "TimeUtils.h"

class WidgetClockConf : public AbstractWidget<ModelClock>
{
    public:
        WidgetClockConf(lv_group_t* wGroup, QueueHandle_t& mEventQueue) 
            : AbstractWidget(wGroup, mEventQueue) {}
        
        ~WidgetClockConf() override {

            // ESP_LOGD(TAG_WIDGT, "WidgetClockConf destructor start");
            if (lv_obj_is_valid(dropDownHours)) {
                lv_obj_del(dropDownHours);
            }
             if (lv_obj_is_valid(dropDownMinutes)) {
                lv_obj_del(dropDownMinutes);
            }
            // lv_style_reset(&styleTime);

            ESP_LOGD(TAG_WIDGT, "WidgetClockConf destructor end");
        }
    
    private: 

        // TODO: Delete buttons
        lv_obj_t* labelHours = nullptr;
        lv_obj_t* dropDownHours = nullptr;

        lv_obj_t* labelMinutes = nullptr;
        lv_obj_t* dropDownMinutes = nullptr;

        lv_obj_t* labelSeconds = nullptr;
        lv_obj_t* dropDownSeconds = nullptr;

        lv_obj_t* labelAmPm = nullptr;
        lv_obj_t* dropDownAmPm = nullptr;

        lv_obj_t* labelDay = nullptr;
        lv_obj_t* dropDownDay = nullptr;

        lv_obj_t* labelMonth = nullptr;
        lv_obj_t* dropDownMonth = nullptr;

        lv_obj_t* labelYear = nullptr;
        lv_obj_t* dropDownYear = nullptr;

        lv_obj_t* buttonSave = nullptr;
        lv_obj_t* labelButtonSave = nullptr;

        lv_obj_t* buttonClose = nullptr;
        lv_obj_t* labelButtonClose = nullptr;

        lv_style_t styleDropDownFocused;
        lv_style_t styleButton;
        lv_style_t styleButtonFocused;

        DBox createBox(ModelClock& widgetData) override
        {
            return widgetData.box;
        }

        void initialize(ModelClock& widgetData) override {

            // Set parent font
            lv_style_set_text_font(&style, &lv_font_montserrat_18);

            // Init style for focused dropdown
            lv_style_init(&styleDropDownFocused);
            lv_style_set_border_width(&styleDropDownFocused, 2);
            lv_style_set_border_color(&styleDropDownFocused, lv_color_black());
            lv_style_set_border_side(&styleDropDownFocused, LV_BORDER_SIDE_BOTTOM);
            lv_style_set_radius(&styleDropDownFocused, 0);

            // Init style for focused button
            lv_style_init(&styleButton);
            lv_style_set_text_decor(&styleButton, LV_TEXT_DECOR_UNDERLINE);
            lv_style_set_text_color(&styleButton, lv_color_black());
            lv_style_set_bg_color(&styleButton, lv_color_white());
            lv_style_set_bg_opa(&styleButton, LV_OPA_COVER);
            lv_style_set_border_width(&styleButton, 2);
            lv_style_set_border_color(&styleButton, lv_color_black());
            lv_style_set_border_opa(&styleButton, LV_OPA_COVER);

            lv_style_init(&styleButtonFocused);
            lv_style_set_text_decor(&styleButtonFocused, LV_TEXT_DECOR_UNDERLINE);
            lv_style_set_text_color(&styleButtonFocused, lv_color_white());
            lv_style_set_bg_color(&styleButtonFocused, lv_color_black());
            lv_style_set_bg_opa(&styleButtonFocused, LV_OPA_COVER);
            // lv_style_set_border_width(&styleButtonFocused, 2);
            lv_style_set_border_color(&styleButtonFocused, lv_color_black());
            lv_style_set_border_opa(&styleButtonFocused, LV_OPA_COVER);

            dropDownHours = lv_dropdown_create(parent);
            lv_obj_set_pos(dropDownHours, 0, 16);
            lv_obj_set_width(dropDownHours, 104);
            lv_dropdown_set_options(dropDownHours, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12");
            
            labelHours = lv_label_create(parent);
            lv_label_set_text(labelHours, "Hours");
            lv_obj_align_to(labelHours, dropDownHours, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownMinutes = lv_dropdown_create(parent);
            lv_obj_set_pos(dropDownMinutes, 112, 16);
            lv_obj_set_width(dropDownMinutes, 104);
            lv_dropdown_set_options(dropDownMinutes, "1\n2\n3\n4\n5");

            labelMinutes = lv_label_create(parent);
            lv_label_set_text(labelMinutes, "Minutes");
            lv_obj_align_to(labelMinutes, dropDownMinutes, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownSeconds = lv_dropdown_create(parent);
            lv_obj_set_pos(dropDownSeconds, 224, 16);
            lv_obj_set_width(dropDownSeconds, 104);
            lv_dropdown_set_options(dropDownSeconds, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12");

            labelSeconds = lv_label_create(parent);
            lv_label_set_text(labelSeconds, "Seconds");
            lv_obj_align_to(labelSeconds, dropDownSeconds, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownAmPm = lv_dropdown_create(parent);
            lv_obj_set_pos(dropDownAmPm, 336, 16);
            lv_obj_set_width(dropDownAmPm, 104);
            lv_dropdown_set_options(dropDownAmPm, "AM\nPM");

            labelAmPm = lv_label_create(parent);
            lv_label_set_text(labelAmPm, "AM/PM");
            lv_obj_align_to(labelAmPm, dropDownAmPm, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownDay = lv_dropdown_create(parent);
            lv_obj_set_pos(dropDownDay, 0, 104);
            lv_obj_set_width(dropDownDay, 104);
            lv_dropdown_set_options(dropDownDay, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12");

            labelDay = lv_label_create(parent);
            lv_label_set_text(labelDay, "Day");
            lv_obj_align_to(labelDay, dropDownDay, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownMonth = lv_dropdown_create(parent);
            lv_obj_set_pos(dropDownMonth, 112, 104);
            lv_obj_set_width(dropDownMonth, 156);
            lv_dropdown_set_options(dropDownMonth, "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n11\n12");

            labelMonth = lv_label_create(parent);
            lv_label_set_text(labelMonth, "Month");
            lv_obj_align_to(labelMonth, dropDownMonth, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownYear = lv_dropdown_create(parent);
            lv_obj_set_pos(dropDownYear, 284, 104);
            lv_obj_set_width(dropDownYear, 156);
            lv_dropdown_set_options(dropDownYear, "2025\n2026\n2027");

            labelYear = lv_label_create(parent);
            lv_label_set_text(labelYear, "Year");
            lv_obj_align_to(labelYear, dropDownYear, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            // Buttons
            uint16_t buttonWidth = (box.width - box.padding * 2 - box.border * 2) / 2 - 4;
            buttonSave = lv_btn_create(parent);
            lv_obj_set_pos(buttonSave, 0, 176);
            lv_obj_set_size(buttonSave, buttonWidth, 48);
            
            labelButtonSave = lv_label_create(buttonSave);
            lv_label_set_text(labelButtonSave, "Save");  
            lv_obj_center(labelButtonSave);

            buttonClose = lv_btn_create(parent);
            lv_obj_set_pos(buttonClose, buttonWidth + 8, 176);
            lv_obj_set_size(buttonClose, buttonWidth, 48);
            
            labelButtonClose = lv_label_create(buttonClose);
            lv_label_set_text(labelButtonClose, "Close");  
            lv_obj_center(labelButtonClose);

            lv_group_add_obj(widgetGroup, dropDownHours);
            lv_group_add_obj(widgetGroup, dropDownMinutes);
            lv_group_add_obj(widgetGroup, dropDownSeconds);
            lv_group_add_obj(widgetGroup, dropDownAmPm);
            lv_group_add_obj(widgetGroup, dropDownDay);
            lv_group_add_obj(widgetGroup, dropDownMonth);
            lv_group_add_obj(widgetGroup, dropDownYear);
            lv_group_add_obj(widgetGroup, buttonSave);
            lv_group_add_obj(widgetGroup, buttonClose);

            lv_obj_add_style(dropDownHours,   &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownMinutes, &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownSeconds, &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownAmPm,    &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownDay,     &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownMonth,   &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownYear,    &styleDropDownFocused, LV_STATE_FOCUSED);

            lv_obj_add_style(buttonSave,  &styleButton, LV_PART_MAIN);
            lv_obj_add_style(buttonClose, &styleButton, LV_PART_MAIN);
            lv_obj_add_style(buttonSave,  &styleButtonFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(buttonClose, &styleButtonFocused, LV_STATE_FOCUSED);

            lv_group_focus_obj(dropDownHours);
            
            // Event control
            // Set user data to 0, so controller (intent) understands that exit is pressed)
            lv_obj_set_user_data(buttonClose, (void*)(uintptr_t)0);
            attachEventHandler(buttonClose);
        }

        void print(ModelClock& widgetData) override
        {
           
        }
};

#endif
