#include "lvgl.h"
#include "app_hal.h"
// #include <PinDefinitions.h>

#ifdef ARDUINO
#include <Arduino.h>

void create_black_square(lv_obj_t * parent);
void lv_example_list_1(void);

static lv_obj_t * list1;

void setup()
{
    Serial.begin(115200);

    Serial.println("----- BOOT -----");
    // Init grafics lib
	lv_init();
	hal_setup();

	lv_example_list_1();
}

void loop()
{
	hal_loop();  /* Do not use while loop in this function */
}

void create_black_square(lv_obj_t * parent) {
    // Create a new object (basic rectangle object)
    lv_obj_t * rect = lv_obj_create(parent); 
    
    lv_obj_set_size(rect, 56, 56);
    
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

void lv_example_list_1(void)
{
    /*Create a list*/
    list1 = lv_list_create(lv_screen_active());
    lv_obj_set_size(list1, 256, 512);
    lv_obj_center(list1);

    /*Add buttons to the list*/
    lv_obj_t * btn;
    lv_list_add_text(list1, "File");
    btn = lv_list_add_button(list1, LV_SYMBOL_FILE, "New");
    btn = lv_list_add_button(list1, LV_SYMBOL_DIRECTORY, "Open");
    btn = lv_list_add_button(list1, LV_SYMBOL_SAVE, "Save");
    btn = lv_list_add_button(list1, LV_SYMBOL_CLOSE, "Delete");
    btn = lv_list_add_button(list1, LV_SYMBOL_EDIT, "Edit");

    lv_list_add_text(list1, "Connectivity");
    btn = lv_list_add_button(list1, LV_SYMBOL_BLUETOOTH, "Bluetooth");
    btn = lv_list_add_button(list1, LV_SYMBOL_GPS, "Navigation");
    btn = lv_list_add_button(list1, LV_SYMBOL_USB, "USB");
    btn = lv_list_add_button(list1, LV_SYMBOL_BATTERY_FULL, "Battery");

	static lv_style_t style_font24;
    lv_style_init(&style_font24);

    // Set the text font size
    lv_style_set_text_font(&style_font24, &lv_font_montserrat_24); 

	lv_obj_add_style(list1, &style_font24, LV_PART_MAIN); // Apply to the main part
	lv_obj_add_style(list1, &style_font24, LV_PART_ITEMS); 
}

#else

int main(void)
{
	lv_init();

	hal_setup();

	lv_demo_widgets();

	hal_loop();
}

#endif /*ARDUINO*/