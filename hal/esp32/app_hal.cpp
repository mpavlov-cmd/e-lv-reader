
#include "app_hal.h"
#include "lvgl.h"

#include <Arduino.h>
#include <SPI.h>
#include "SD.h"
#include "FS.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "drvlvgl/Driver_Display_EPD.h"
#include "drvlvgl/Driver_Input_Keypad.h"
#include "drvfs/Driver_Arduino_FS.h"

#include "PinDefinitions.h"
#include "SleepControl.h"
#include "FileManager.h"
#include "AbstractIntent.h"
#include "intent/IntentHome.h"
#include "intent/IntentSleep.h"
#include "intent/IntentFileSelector.h"

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
QueueHandle_t eventQueue = xQueueCreate(256, sizeof(ActionArgument));
QueueHandle_t freqencyQueue = xQueueCreate(1, sizeof(uint8_t));; 

ESP32Time rtc(0);
FileManager fileManager(SD, PIN_CS_SD);

TaskHandle_t intentFreqHandle = NULL;
AbstractIntent* intentCurrent = new IntentHome(eventQueue, rtc, fileManager);


void hal_setup(void)
{
    // TODO: Change for concrete tasks, not global
    // Init and configure wdt with timeot seconds and panic flag
	esp_task_wdt_init(60, false);

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

    void *queues[2] = { eventQueue, freqencyQueue };
    xTaskCreate(eventQueueTask, "uiTask", 4096 * 2, queues, 1, nullptr);
    xTaskCreate(taskIntentFreq, "intentFreq", 2048, freqencyQueue, 1, &intentFreqHandle);
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
    QueueHandle_t queue = static_cast<QueueHandle_t>(pvParameters);
    uint8_t one = 1;

    for(;;) {
        if (xQueueSend(queue, &one, pdMS_TO_TICKS(10)) != pdPASS)
        {
            ESP_LOGV(TAG_MAIN, "Failed to send event to frequency queue");
        }
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void eventQueueTask(void *pvParameters)
{
    // Get queues
    void **queues = (void **)pvParameters;

    QueueHandle_t eventQueue = (QueueHandle_t)queues[0];
    QueueHandle_t freqencyQueue = (QueueHandle_t)queues[1];

    ActionArgument actionArgument;
    uint8_t frequencyArgment;

    while (true)
    {
        // Wait to get item from frequency producer
        if (xQueueReceive(freqencyQueue, &frequencyArgment, pdMS_TO_TICKS(5)) == pdPASS) {
            ESP_LOGD(TAG_MAIN, "Executing inetent frequency task");
            intentCurrent->onFrequncy();
        }

        // Check if actions were prformed
        if (xQueueReceive(eventQueue, &actionArgument, pdMS_TO_TICKS(5)) == pdPASS)
        {
            // Do not feed lvgl until event is processed
            lvglTimerEnabled = false;

            // ESP_LOGD(TAG_MAIN, "Received event: target=%p, code=%d", actionArg.target, actionArg.code);
            ActionResult result = intentCurrent->onAction(actionArgument);

            if (result.type == ActionRetultType::CHANGE_INTENT) {
                ESP_LOGD(TAG_MAIN, "Change intent action fired with id: %i", result.id);
                // Force full refresh on change intent
                lv_epd_mark_full();
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
	case INTENT_ID_FILE_SELECTOR:
		intentCurrent = new IntentFileSelector(eventQueue, fileManager);
		break;
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
