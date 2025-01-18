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

        lv_obj_t* labelHours = nullptr;
        lv_obj_t* dropDownHours = nullptr;
        char optionsHours[128];

        lv_obj_t* labelMinutes = nullptr;
        lv_obj_t* dropDownMinutes = nullptr;
        char optionsMinutes[256];

        lv_obj_t* labelSeconds = nullptr;
        lv_obj_t* dropDownSeconds = nullptr;
        char optionsSeconds[256];

        lv_obj_t* labelDay = nullptr;
        lv_obj_t* dropDownDay = nullptr;
        char optionsDay[128];

        lv_obj_t* labelMonth = nullptr;
        lv_obj_t* dropDownMonth = nullptr;
        char optionsMonth[64];

        lv_obj_t* labelYear = nullptr;
        lv_obj_t* dropDownYear = nullptr;
        char optionsYear[256];

        lv_obj_t* buttonSave = nullptr;
        lv_obj_t* labelButtonSave = nullptr;

        lv_obj_t* buttonClose = nullptr;
        lv_obj_t* labelButtonClose = nullptr;

        lv_style_t styleDropDownFocused;
        lv_style_t styleButton;
        lv_style_t styleButtonFocused;

        // Options generator
        void generateStringSeq(uint16_t start, uint16_t end, char* result, size_t length) {
            // Check if input is valid
            if (start > end || length == 0) {
                if (length > 0) {
                    result[0] = '\0'; // Return empty string for invalid inputs
                }
                return;
            }

            size_t pos = 0; // Current position in the result array
            for (uint16_t i = start; i <= end; i++) {
                // Convert the number to a string and calculate the length
                char buffer[5]; // Buffer to store the string representation of the number
                int numLength = snprintf(buffer, sizeof(buffer), "%d", i);

                // Check if there's enough space in the result array for the number and delimiter
                if (pos + numLength + ((i < end) ? 1 : 0) >= length) {
                    result[0] = '\0'; // Not enough space, return empty string
                    return;
                }

                // Append the number to the result
                strncpy(result + pos, buffer, numLength);
                pos += numLength;

                // Append the newline character except for the last number
                if (i < end) {
                    result[pos] = '\n';
                    pos++;
                }
            }

            // Null-terminate the string
            result[pos] = '\0';
        }

        int16_t findValueInString(const char *inputString, uint16_t number)
        {
            if (!inputString) {
                return -1; // Handle null pointer case
            }

            char buffer[6];                                 // Temporary buffer to hold the string representation of the number
            snprintf(buffer, sizeof(buffer), "%u", number); // Convert the number to a string

            const char *pos = inputString; // Pointer to traverse the input string
            int16_t index = 0;             // Index counter for the numbers in the string

            while (*pos != '\0')
            {
                // Compare the number at the current position
                if (strncmp(pos, buffer, strlen(buffer)) == 0)
                {
                    // Ensure it matches fully (check for number boundaries)
                    char nextChar = *(pos + strlen(buffer));
                    if (nextChar == '\n' || nextChar == '\0')
                    {
                        return index;
                    }
                }

                // Move to the next number by finding the next newline or end of string
                while (*pos != '\n' && *pos != '\0') {
                    pos++;
                }
                if (*pos == '\n') {
                    pos++; // Skip newline if found
                }
                index++;
            }

            return -1; // Return -1 if the number is not found
        }

        // Common event handler
        static void dropDownEventHandler(lv_event_t *event)
        {
            lv_obj_t *target = lv_event_get_target(event);
            lv_event_code_t code = lv_event_get_code(event);

            ESP_LOGD(TAG_WIDGT, "Received dropdown event before cast");

            // Retrieve the widget instance and its event queue (passed as user data)
            WidgetClockConf* widget = static_cast<WidgetClockConf*>(lv_event_get_user_data(event));
            if (!widget)
            {
                return;
            }

            ESP_LOGD(TAG_WIDGT, "Received dropdown event");

        }

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
            generateStringSeq(0, 23, optionsHours, sizeof(optionsHours));
            lv_obj_set_pos(dropDownHours, 0, 16);
            lv_obj_set_width(dropDownHours, 104);
            lv_dropdown_set_options(dropDownHours, optionsHours);
            lv_dropdown_set_selected(dropDownHours, findValueInString(optionsHours, widgetData.hour));
            
            labelHours = lv_label_create(parent);
            lv_label_set_text(labelHours, "Hours");
            lv_obj_align_to(labelHours, dropDownHours, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownMinutes = lv_dropdown_create(parent);
            generateStringSeq(0, 59, optionsMinutes, sizeof(optionsMinutes));
            lv_obj_set_pos(dropDownMinutes, 112, 16);
            lv_obj_set_width(dropDownMinutes, 156);
            lv_dropdown_set_options(dropDownMinutes, optionsMinutes);
            lv_dropdown_set_selected(dropDownMinutes, findValueInString(optionsMinutes, widgetData.min));

            labelMinutes = lv_label_create(parent);
            lv_label_set_text(labelMinutes, "Minutes");
            lv_obj_align_to(labelMinutes, dropDownMinutes, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownSeconds = lv_dropdown_create(parent);
            generateStringSeq(0, 59, optionsSeconds, sizeof(optionsSeconds));
            lv_obj_set_pos(dropDownSeconds, 284, 16);
            lv_obj_set_width(dropDownSeconds, 156);
            lv_dropdown_set_options(dropDownSeconds, optionsSeconds);
            lv_dropdown_set_selected(dropDownSeconds, findValueInString(optionsSeconds, widgetData.sec));

            labelSeconds = lv_label_create(parent);
            lv_label_set_text(labelSeconds, "Seconds");
            lv_obj_align_to(labelSeconds, dropDownSeconds, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownDay = lv_dropdown_create(parent);
            generateStringSeq(1, 31, optionsDay, sizeof(optionsDay)); // TODO: num of days based on the month
            lv_obj_set_pos(dropDownDay, 0, 104);
            lv_obj_set_width(dropDownDay, 104);
            lv_dropdown_set_options(dropDownDay, optionsDay);
            lv_dropdown_set_selected(dropDownDay, findValueInString(optionsDay, widgetData.day));

            labelDay = lv_label_create(parent);
            lv_label_set_text(labelDay, "Day");
            lv_obj_align_to(labelDay, dropDownDay, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownMonth = lv_dropdown_create(parent);
            generateStringSeq(1, 12, optionsMonth, sizeof(optionsMonth));
            lv_obj_set_pos(dropDownMonth, 112, 104);
            lv_obj_set_width(dropDownMonth, 156);
            lv_dropdown_set_options(dropDownMonth, optionsMonth);
            lv_dropdown_set_selected(dropDownMonth, findValueInString(optionsMonth, widgetData.month));

            labelMonth = lv_label_create(parent);
            lv_label_set_text(labelMonth, "Month");
            lv_obj_align_to(labelMonth, dropDownMonth, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            dropDownYear = lv_dropdown_create(parent);
            generateStringSeq(2024, 2035, optionsYear, sizeof(optionsYear));
            lv_obj_set_pos(dropDownYear, 284, 104);
            lv_obj_set_width(dropDownYear, 156);
            lv_dropdown_set_options(dropDownYear, optionsYear);
            lv_dropdown_set_selected(dropDownYear, findValueInString(optionsYear, widgetData.year));

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

            // TODO: Add dropdowns to array to loop through them
            lv_group_add_obj(widgetGroup, dropDownHours);
            lv_group_add_obj(widgetGroup, dropDownMinutes);
            lv_group_add_obj(widgetGroup, dropDownSeconds);
            lv_group_add_obj(widgetGroup, dropDownDay);
            lv_group_add_obj(widgetGroup, dropDownMonth);
            lv_group_add_obj(widgetGroup, dropDownYear);
            lv_group_add_obj(widgetGroup, buttonSave);
            lv_group_add_obj(widgetGroup, buttonClose);

            lv_obj_add_style(dropDownHours,   &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownMinutes, &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownSeconds, &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownDay,     &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownMonth,   &styleDropDownFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(dropDownYear,    &styleDropDownFocused, LV_STATE_FOCUSED);

            lv_obj_add_event_cb(dropDownHours,   dropDownEventHandler, LV_EVENT_VALUE_CHANGED, this);
            lv_obj_add_event_cb(dropDownMinutes, dropDownEventHandler, LV_EVENT_VALUE_CHANGED, this);
            lv_obj_add_event_cb(dropDownSeconds, dropDownEventHandler, LV_EVENT_VALUE_CHANGED, this);
            lv_obj_add_event_cb(dropDownDay,     dropDownEventHandler, LV_EVENT_VALUE_CHANGED, this);
            lv_obj_add_event_cb(dropDownMonth,   dropDownEventHandler, LV_EVENT_VALUE_CHANGED, this);
            lv_obj_add_event_cb(dropDownYear,    dropDownEventHandler, LV_EVENT_VALUE_CHANGED, this);

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
           // Do nothing
        }
};

#endif
