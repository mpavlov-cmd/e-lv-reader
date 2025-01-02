#include "lvgl.h"
#include "app_hal.h"
#include "esp_log.h"

#ifdef ARDUINO
#include <Arduino.h>

const char* TAG_MAIN = "MAIN";

void setup()
{
    Serial.begin(115200);
	ESP_LOGI(TAG_MAIN, "----- BOOT SUCCESS -----");

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