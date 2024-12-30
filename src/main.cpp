#include "lvgl.h"
#include "app_hal.h"

#include "demos/lv_demos.h"

#ifdef ARDUINO
#include <Arduino.h>

void create_black_square(lv_obj_t * parent);

void setup()
{
	// ---- GENERC DRIVER TEST START ----



	// EPD_HW_Init();			 // Full screen refresh initialization.
	// EPD_WhiteScreen_White(); // Clear screen function.
	// EPD_DeepSleep();		 // Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
	// delay(2000);			 // Delay for 2s.
	// /************Full display(2s)*******************/
	// EPD_HW_Init();				   // Full screen refresh initialization.
	// EPD_WhiteScreen_ALL(gImage_1); // To Display one image using full screen refresh.
	// EPD_DeepSleep();			   // Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
	// delay(2000);				   // Delay for 2s.

	/************Fast refresh mode(1.5s)*******************/
	// EPD_HW_Init();							 // Electronic paper initialization.
	// EPD_SetRAMValue_BaseMap(gImage_basemap); // Please do not delete the background color function, otherwise it will cause unstable display during partial refresh.
	// for (int i = 0; i < 6; i++)
	// 	EPD_Dis_Part_Time(320, 124 + 48 * 0, Num[1],		   // x-A,y-A,DATA-A
	// 					  320, 124 + 48 * 1, Num[0],		   // x-B,y-B,DATA-B
	// 					  320, 124 + 48 * 2, gImage_numdot,	   // x-C,y-C,DATA-C
	// 					  320, 124 + 48 * 3, Num[0],		   // x-D,y-D,DATA-D
	// 					  320, 124 + 48 * 4, Num[i], 48, 104); // x-E,y-E,DATA-E,Resolution  32*64

	// EPD_DeepSleep();		 // Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
	// delay(2000);			 // Delay for 2s.
	// EPD_HW_Init();			 // Full screen refresh initialization.
	// EPD_WhiteScreen_White(); // Clear screen function.
	// EPD_DeepSleep();		 // Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
	// delay(2000);			 // Delay for 2s.

	// ---- GENERC DRIVER TEST END   ----

	lv_init();
	hal_setup();

	create_black_square(lv_scr_act());
	// lv_demo_widgets();
}

void loop()
{
	hal_loop();  /* Do not use while loop in this function */
}

void create_black_square(lv_obj_t * parent) {
    // Create a new object (basic rectangle object)
    lv_obj_t * rect = lv_obj_create(parent); 
    
    lv_obj_set_size(rect, 24, 24);
    
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

#else

int main(void)
{
	lv_init();

	hal_setup();

	lv_demo_widgets();

	hal_loop();
}

#endif /*ARDUINO*/