
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
#include "SleepControl.h"
#include "FileManager.h"
#include "AbstractIntent.h"
#include "intent/IntentHome.h"
#include "intent/IntentSleep.h"

// Function definitions
void blink(void* pvParameters);
void taskIntentFreq(void* pvParameters);
void eventQueueTask(void* pvParameters);
void buildIntent(uint8_t intentId);
void switchIntent(uint8_t intentId, IntentArgument intentArgument);

// Variable definitions
SleepControlConf sleepCtrlConf = {GPIO_SEL_34 | GPIO_SEL_36 | GPIO_SEL_39, ESP_EXT1_WAKEUP_ALL_LOW};
SleepControl sleepControl(sleepCtrlConf);

// Initialize Event Queue for MVC logc 
volatile bool lvglTimerEnabled = true;
QueueHandle_t eventQueue = xQueueCreate(256, sizeof(ActionArgument));; 

ESP32Time rtc(0);
FileManager fileManager(SD, PIN_CS_SD);

TaskHandle_t intentFreqHandle = NULL;
AbstractIntent* intentCurrent = new IntentHome(eventQueue, rtc, fileManager);


void hal_setup(void)
{

    xTaskCreate(blink, "blinky", 4096, NULL, 5, NULL);
    sleepControl.configureExt1WakeUp();

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

    xTaskCreatePinnedToCore(eventQueueTask, "uiTask", 4096 * 2, eventQueue, 1, nullptr, 0);
    xTaskCreate(taskIntentFreq, "intentFreq", 4096, NULL, 1, &intentFreqHandle);
}

void hal_loop(void)
{
    ESP_LOGV(TAG_MAIN, "Hello from loop");
    delay(1000);
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
        lvglTimerEnabled = false;
		intentCurrent->onFrequncy();
        lvglTimerEnabled = true;
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
        if (xQueueReceive(eventQueue, &actionArg, pdMS_TO_TICKS(5)) == pdPASS)
        {
            // Do not feed lvgl until event is processed
            lvglTimerEnabled = false;

            // ESP_LOGD(TAG_MAIN, "Received event: target=%p, code=%d", actionArg.target, actionArg.code);
            ActionResult result = intentCurrent->onAction(actionArg);

            if (result.type == ActionRetultType::CHANGE_INTENT) {
                ESP_LOGD(TAG_MAIN, "Change intent action fired with id: %i", result.id);
                switchIntent(result.id, result.data);
		    }

            lvglTimerEnabled = true;
        }

        // Update the UI
        lv_timer_handler(); 
    }
}

void buildIntent(uint8_t intentId)
{
	switch (intentId)
	{
	case INTENT_ID_HOME:
		intentCurrent = new IntentHome(eventQueue, rtc, fileManager);
		break;
	// case INTENT_ID_FILE_SELECTOR:
	// 	intentCurrent = new IntentFileSelector(display, fileManager);
	// 	break;
	case INTENT_ID_SLEEP:
		intentCurrent = new IntentSleep(eventQueue, sleepControl);
		break;
	// case INTENT_ID_BOOK:
	// 	intentCurrent = new IntentBook(display, semaphoreHandle, textIndex, fileManager);
	// 	break;
	default:
		intentCurrent = new IntentHome(eventQueue, rtc, fileManager);
		break;
	}
}

void switchIntent(uint8_t intentId, IntentArgument intentArgument)
{
    intentCurrent->onExit();
	delete intentCurrent;

	// Init new intent based on the id under the hood will assign new intent to intent current
	buildIntent(intentId);
	intentCurrent->onStartUp(intentArgument);
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
