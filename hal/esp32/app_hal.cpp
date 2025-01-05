
#include "app_hal.h"
#include "lvgl.h"

#include <Arduino.h>
#include <SPI.h>
#include "SD.h"
#include "FS.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "drvlvgl/Driver_Display_EPD.h"
#include "drvlvgl/Driver_Input_Keypad.h"
#include "drvfs/Driver_Arduino_FS.h"

#include "PinDefinitions.h"
#include "FileManager.h"
#include "AbstractIntent.h"
#include "intent/IntentHome.h"

// Function definitions
void blink(void* pvParameters);
void taskIntentFreq(void* pvParameters);
void eventQueueTask(void* pvParameters);

// Variable definitions
// Initialize Event Queue for MVC logc 
QueueHandle_t eventQueue = xQueueCreate(10, sizeof(ActionArgument));; 

ESP32Time rtc(0);
FileManager fileManager(SD, PIN_CS_SD);

TaskHandle_t intentFreqHandle = NULL;
AbstractIntent* intentCurrent = new IntentHome(eventQueue, rtc, fileManager);


void hal_setup(void)
{
    xTaskCreate(blink, "blinky", 4096, NULL, 5, NULL);

    // SPI
    // SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    SPI.begin();

    // File manager
    fileManager.begin();

    // Init grafics lib
	lv_init();
    lv_epd_disp_init();
    lv_joystick_indev_init();

    // Init lv file system 
    lv_arduino_fs_init();


    // Lunch intent mechaism
    intentCurrent->onStartUp(IntentArgument::NO_ARG);

    xTaskCreate(eventQueueTask, "eventQueueTask", 2048, eventQueue, 1, nullptr);
    xTaskCreate(taskIntentFreq, "intentFreq", 4096, NULL, 1, &intentFreqHandle);
}

void hal_loop(void)
{
    // Update the UI
    lv_timer_handler(); 
    delay(5);
}

void blink(void *pvParameters) {
	pinMode(PIN_LED, OUTPUT);
	for (;;) {
		// Blink
		digitalWrite(PIN_LED, HIGH);
		vTaskDelay(500 / portTICK_RATE_MS);
		digitalWrite(PIN_LED, LOW);
		vTaskDelay(500 / portTICK_RATE_MS);
    }
}

void taskIntentFreq(void *pvParameters)
{
    for(;;) {
		intentCurrent->onFrequncy();
		vTaskDelay(10000 / portTICK_RATE_MS);
	}
}

void eventQueueTask(void *pvParameters)
{
    QueueHandle_t eventQueue = static_cast<QueueHandle_t>(pvParameters);
    ActionArgument actionArg;

    while (true)
    {
        // Wait indefinitely for an event
        if (xQueueReceive(eventQueue, &actionArg, portMAX_DELAY) == pdPASS)
        {
            // ESP_LOGD(TAG_MAIN, "Received event: target=%p, code=%d", actionArg.target, actionArg.code);
            intentCurrent->onAction(actionArg);
        }
    }
}

// To be moved:
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
