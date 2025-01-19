#ifndef WIDGETCLOCKCONF_H
#define WIDGETCLOCKCONF_H

#include "AbstractWidget.h"
#include "model/clock/ModelClock.h"
#include "TimeUtils.h"
#include "utils/DropDownUtils.h"

#define IDX_CONF_HOUR 0
#define IDX_CONF_MIN  1
#define IDX_CONF_SEC  2
#define IDX_CONF_DAY  3
#define IDX_CONF_MTH  4
#define IDX_CONF_YAR  5

class WidgetClockConf : public AbstractWidget<ModelClock>
{
    public:

        struct Action {
            uint8_t id;
            ModelClock* data;
        };

        WidgetClockConf(lv_group_t* wGroup, QueueHandle_t& mEventQueue) 
            : AbstractWidget(wGroup, mEventQueue) {}
        
        // TODO: Delete remaining values
        ~WidgetClockConf() override {
            
            // Delete actions
            delete actionClose;
            delete actionSave;

            // Parent deletion should take care of childrn
            if (lv_obj_is_valid(parent)) {
                lv_obj_del(parent);
            }
             
            lv_style_reset(&styleDropDownFocused);
            lv_style_reset(&styleButton);
            lv_style_reset(&styleButtonFocused);

            ESP_LOGD(TAG_WIDGT, "WidgetClockConf destructor end");
        }
    
    private: 
        // Define actions 
        Action *actionClose = nullptr, *actionSave = nullptr;

        // Define labels
        lv_obj_t *labelHours = nullptr, *labelMinutes = nullptr, *labelSeconds = nullptr;
        lv_obj_t *labelDay = nullptr, *labelMonth = nullptr, *labelYear = nullptr;

        // Define drop-downs
        lv_obj_t *dropDownHours = nullptr, *dropDownMinutes = nullptr, *dropDownSeconds = nullptr;
        lv_obj_t *dropDownDay = nullptr, *dropDownMonth = nullptr, *dropDownYear = nullptr;
        lv_obj_t *dropDowns[6] = {nullptr};

        // Define option values
        char optionsHours[128], optionsMinutes[256], optionsSeconds[256], optionsDay[128], optionsMonth[64], optionsYear[256];

        // Define buttons and button labels
        lv_obj_t *buttonSave = nullptr, *buttonClose = nullptr;
        lv_obj_t *labelButtonSave = nullptr, *labelButtonClose = nullptr;

        // Define styles
        lv_style_t styleDropDownFocused, styleButton, styleButtonFocused;

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

            lv_obj_t* dropdown = lv_event_get_target(event); 
            const uint8_t inputIndex = (uint8_t)(uintptr_t)lv_obj_get_user_data(dropdown);

            // Get ref for model fo simplicity
            ModelClock* model = widget->actionSave->data;

            // Set currect days in month
            if (inputIndex == IDX_CONF_MTH || inputIndex == IDX_CONF_YAR) {
                
                // Get currently selected id option for days
                uint8_t selectedDayId = lv_dropdown_get_selected(widget->dropDownDay);

                // Get currently selected option
                char selDayString[3];
                lv_dropdown_get_selected_str(widget->dropDownDay, selDayString, sizeof(selDayString)); 
                uint8_t currentDayValue = (uint8_t)atoi(selDayString);
                
                // Get selectd month
                char selMonthString[3];
                lv_dropdown_get_selected_str(widget->dropDownMonth, selMonthString, sizeof(selMonthString)); 
                uint8_t newMondthValue = (uint8_t)atoi(selMonthString);

                // Get selected year
                char selYearString[5];
                lv_dropdown_get_selected_str(widget->dropDownYear, selYearString, sizeof(selYearString)); 
                uint16_t newYearValue = (uint16_t)atoi(selYearString);

                // Modify number of days in the year
                uint8_t daysInMonth = maxDaysInMonth(newMondthValue, newYearValue);
                generateStringSeq(1, daysInMonth, widget->optionsDay, sizeof(widget->optionsDay));
                lv_dropdown_set_options(widget->dropDownDay, widget->optionsDay);

                uint8_t selectedId = currentDayValue > daysInMonth ? 0 : selectedDayId;
                lv_dropdown_set_selected(widget->dropDownDay, selectedId);

                // Set new values for model
                model->month = newMondthValue; 
                model->year  = newYearValue;
            }

            // Set model values for other components
            char selValString[3];
            
            if (inputIndex == IDX_CONF_HOUR) {
                lv_dropdown_get_selected_str(widget->dropDownHours, selValString, sizeof(selValString));
                uint8_t newHourValue = (uint8_t)atoi(selValString);
                model->hour = newHourValue;
            }

            if (inputIndex == IDX_CONF_MIN) {
                lv_dropdown_get_selected_str(widget->dropDownMinutes, selValString, sizeof(selValString));
                uint8_t newMinValue = (uint8_t)atoi(selValString);
                model->min = newMinValue;
            }

            if (inputIndex == IDX_CONF_SEC) {
                lv_dropdown_get_selected_str(widget->dropDownSeconds, selValString, sizeof(selValString));
                uint8_t newSecValue = (uint8_t)atoi(selValString);
                model->sec = newSecValue;
            }

            if (inputIndex == IDX_CONF_DAY) {
                lv_dropdown_get_selected_str(widget->dropDownDay, selValString, sizeof(selValString));
                uint8_t newDayValue = (uint8_t)atoi(selValString);
                model->day = newDayValue;
            }
           
            memset(selValString, 0, sizeof(selValString));

            // Log seleted value
            ESP_LOGD(
                TAG_WIDGT, "%i:%i:%i %i/%i/%i", 
                model->hour, model->min, model->sec, model->day, model->month, model->year
            );
        }

        void createDropDown(
            lv_obj_t** result, lv_obj_t* parent, uint8_t pos,
            int optMin, int optMax, char* optString, size_t optSize,
            uint16_t x, uint16_t y, uint16_t width, int value
        ) {
            *result = lv_dropdown_create(parent);
            dropDowns[pos] = *result;
            generateStringSeq(optMin, optMax, optString, optSize);
            lv_obj_set_user_data(*result, (void*)(uintptr_t)pos);
            lv_obj_set_pos(*result, x, y);
            lv_obj_set_width(*result, width);
            lv_dropdown_set_options(*result, optString);
            lv_dropdown_set_selected(*result, findValueInString(optString, value));
        }

        void createButtonStyle(lv_style_t *style, lv_color_t textColor, lv_color_t bgColor, uint8_t border)
        {
            lv_style_init(style);
            lv_style_set_text_decor(style, LV_TEXT_DECOR_UNDERLINE);
            lv_style_set_text_color(style, textColor);
            lv_style_set_bg_color(style, bgColor);
            lv_style_set_bg_opa(style, LV_OPA_COVER);
            lv_style_set_border_width(style, border);
            lv_style_set_border_color(style, lv_color_black());
            lv_style_set_border_opa(style, LV_OPA_COVER);
        }

        DBox createBox(ModelClock& widgetData) override
        {
            return widgetData.box;
        }

        void initialize(ModelClock& widgetData) override {

            // Init actions
            actionSave  = new Action {1, &widgetData}; // Important as it is used in actions
            actionClose = new Action {0, nullptr};

            // Set parent font
            lv_style_set_text_font(&style, &lv_font_montserrat_18);

            // Init style for focused dropdown
            lv_style_init(&styleDropDownFocused);
            lv_style_set_border_width(&styleDropDownFocused, 2);
            lv_style_set_border_color(&styleDropDownFocused, lv_color_black());
            lv_style_set_border_side(&styleDropDownFocused, LV_BORDER_SIDE_BOTTOM);
            lv_style_set_radius(&styleDropDownFocused, 0);

            // Init button styles 
            createButtonStyle(&styleButton, lv_color_black(), lv_color_white(), 2);
            createButtonStyle(&styleButtonFocused, lv_color_white(), lv_color_black(), 0);

            // Init dropdowns
            uint8_t daysInMonth = maxDaysInMonth(widgetData.month, widgetData.year);

            createDropDown(&dropDownHours, parent, IDX_CONF_HOUR, 0, 23, optionsHours, sizeof(optionsHours),
                0,   16, 104, widgetData.hour);
            createDropDown(&dropDownMinutes, parent, IDX_CONF_MIN, 0, 59, optionsMinutes, sizeof(optionsMinutes),
                112, 16, 156, widgetData.min);
            createDropDown(&dropDownSeconds, parent, IDX_CONF_SEC, 0, 59, optionsSeconds, sizeof(optionsSeconds),
                284, 16, 156, widgetData.sec);
            createDropDown(&dropDownDay, parent, IDX_CONF_DAY, 1, daysInMonth, optionsDay, sizeof(optionsDay),
                0,   104, 104, widgetData.day);
            createDropDown(&dropDownMonth, parent, IDX_CONF_MTH, 1, 12, optionsMonth, sizeof(optionsMonth),
                112, 104, 156, widgetData.month);
            createDropDown(&dropDownYear, parent, IDX_CONF_YAR, 2024, 2035, optionsYear, sizeof(optionsYear),
                284, 104, 156, widgetData.year);
            
            // TODO: DRY, create a function to handle label creation
            labelHours = lv_label_create(parent);
            lv_label_set_text(labelHours, "Hours");
            lv_obj_align_to(labelHours, dropDownHours, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            labelMinutes = lv_label_create(parent);
            lv_label_set_text(labelMinutes, "Minutes");
            lv_obj_align_to(labelMinutes, dropDownMinutes, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            labelSeconds = lv_label_create(parent);
            lv_label_set_text(labelSeconds, "Seconds");
            lv_obj_align_to(labelSeconds, dropDownSeconds, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            labelDay = lv_label_create(parent);
            lv_label_set_text(labelDay, "Day");
            lv_obj_align_to(labelDay, dropDownDay, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

            labelMonth = lv_label_create(parent);
            lv_label_set_text(labelMonth, "Month");
            lv_obj_align_to(labelMonth, dropDownMonth, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

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
    
            for (int i = 0; i < 6; i++) {
                lv_group_add_obj(widgetGroup, dropDowns[i]);
                lv_obj_add_style(dropDowns[i], &styleDropDownFocused, LV_STATE_FOCUSED);
                lv_obj_add_event_cb(dropDowns[i], dropDownEventHandler, LV_EVENT_VALUE_CHANGED, this);
            }

            // Handle buttons
            lv_group_add_obj(widgetGroup, buttonSave);
            lv_group_add_obj(widgetGroup, buttonClose);

            lv_obj_add_style(buttonSave,  &styleButton, LV_PART_MAIN);
            lv_obj_add_style(buttonClose, &styleButton, LV_PART_MAIN);
            lv_obj_add_style(buttonSave,  &styleButtonFocused, LV_STATE_FOCUSED);
            lv_obj_add_style(buttonClose, &styleButtonFocused, LV_STATE_FOCUSED);
            
            lv_obj_set_user_data(buttonClose, actionClose);
            lv_obj_set_user_data(buttonSave, actionSave);

            attachEventHandler(buttonClose);
            attachEventHandler(buttonSave);

            // Focus initial drop-down
            lv_group_focus_obj(dropDownHours);
        }

        void print(ModelClock& widgetData) override
        {
           // Do nothing
        }
};

#endif
