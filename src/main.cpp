#ifdef CORE_DEBUG_LEVEL
#undef CORE_DEBUG_LEVEL
#endif

#define CORE_DEBUG_LEVEL 4
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG

#include "lvgl.h"
#include "app_hal.h"
#include "esp_log.h"

#ifdef ARDUINO
#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
	ESP_LOGD("TAG", "BOOT FROM LOG");

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