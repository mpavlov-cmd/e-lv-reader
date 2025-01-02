
#include "app_hal.h"
#include "lvgl.h"

#include <Arduino.h>
#include "drvlvgl/Driver_Display_EPD.h"
#include "drvlvgl/Driver_Input_Keypad.h"

void tick_timer_callback(void *arg) {
    lv_tick_inc(1); // Increment LVGL tick by 1 ms
}

void create_black_square(lv_obj_t * parent) {
    // Create a new object (basic rectangle object)
    lv_obj_t * rect = lv_obj_create(parent); 
    
    lv_obj_set_size(rect, 128, 128);
    
    // Set the position of the rectangle (optional)
    lv_obj_set_pos(rect, 0, 0); // Adjust as needed
    
    // Create a style for the rectangle
    static lv_style_t style_black;
    lv_style_init(&style_black);
    
    // Set the background color to black
    lv_style_set_bg_color(&style_black, lv_color_make(0, 0, 0));
    lv_style_set_bg_opa(&style_black, LV_OPA_COVER); // Ensure it's fully opaque

    // Apply the style to the rectangle
    lv_obj_add_style(rect, &style_black, 0);
}

static void event_handler(lv_event_t * e) {
    Serial.println("Button Event!");
    lv_obj_t *widget = lv_event_get_target(e);

    lv_obj_t *parent_obj = lv_obj_get_parent(widget);
  
    // create_black_square(lv_scr_act());
    // lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t * obj = lv_event_get_target(e);
    // if(code == LV_EVENT_CLICKED) {
    // }
}

void lv_example_list_1(void)
{
    lv_group_t * widget_group = lv_group_create();

    /*Create a list*/
    lv_obj_t * list1 = lv_list_create(lv_scr_act());
    lv_obj_set_size(list1, 256, 512);
    lv_obj_center(list1);

    /*Add buttons to the list*/
    lv_list_add_text(list1, "File");

    lv_obj_t * btn1 = lv_list_add_btn(list1, LV_SYMBOL_FILE, "New");
    lv_group_add_obj(widget_group, btn1);
    lv_obj_add_state(btn1, LV_STATE_CHECKED);
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t * btn2 = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, "Open");
    lv_group_add_obj(widget_group, btn2);
    lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t * btn3 = lv_list_add_btn(list1, LV_SYMBOL_SAVE, "Save");
    lv_group_add_obj(widget_group, btn3);
    lv_obj_add_event_cb(btn3, event_handler, LV_EVENT_CLICKED, NULL);

	static lv_style_t style_font24;
    lv_style_init(&style_font24);

    // Set the text font size
    lv_style_set_text_font(&style_font24, &lv_font_montserrat_24); 

	lv_obj_add_style(list1, &style_font24, LV_PART_MAIN); // Apply to the main part
	lv_obj_add_style(list1, &style_font24, LV_PART_ITEMS);

    // Button control 
    lv_indev_set_group(get_lv_keypad(), widget_group);
}

void hal_setup(void)
{
    lv_epd_disp_init();
    lv_joystick_indev_init();

    // Set up the tick timer
    const esp_timer_create_args_t tick_timer_args = {
        .callback = &tick_timer_callback,
        .name = "lv_tick_timer"
    };
    esp_timer_handle_t tick_timer;
    esp_timer_create(&tick_timer_args, &tick_timer);
    esp_timer_start_periodic(tick_timer, 1000); // 1ms interval

    // ----- INPUT DRIVER INIT END -----
    // create_black_square(lv_disp_get_scr_act(disp));
    lv_example_list_1();
}

void hal_loop(void)
{
    // Update the UI
    lv_timer_handler(); 
    delay(5);
}
