#include "lvgl.h"
#include "app_hal.h"

#ifdef ARDUINO
#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
    Serial.println("----- BOOT SUCCESS -----");

    // Init grafics lib
	lv_init();
	hal_setup();
}

void loop()
{
	// Do not use while loop in this function
	hal_loop();  
}


#else

int main(void)
{
	lv_init();

	hal_setup();

	lv_demo_widgets();

	hal_loop();
}

#endif