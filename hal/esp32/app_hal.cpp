#ifndef PIO_UNIT_TESTING

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
#include "PowerStatus.h"
#include "text/TextIndex.h"
#include "cache/DirectoryCache.h"

// Intents 
#include "AbstractIntent.h"
#include "intent/IntentHome.h"
#include "intent/IntentSleep.h"
#include "intent/IntentFileSelector.h"
#include "intent/IntentBook.h"
#include "intent/IntentConf.h"

#include "status/StatusManager.h"

#define STATUS_FREQUENCY_MILLS 10 * 1000
#define SLEEP_TIMOUT_MILLS     30 * 1000

// Function definitions
void blink(void* pvParameters);
void taskIntentFreq(void* pvParameters);
void eventQueueTask(void* pvParameters);
void buildIntent(uint8_t intentId);
void switchIntent(uint8_t intentId, IntentArgument intentArgument);

// TODO: For screen testing purp
void create_black_square(lv_obj_t * parent);

// Variable definitions
esp_sleep_wakeup_cause_t wakeUpReason = ESP_SLEEP_WAKEUP_UNDEFINED;

// Initialize Event Queue for MVC logc 
QueueHandle_t eventQueue      = xQueueCreate(256, sizeof(ActionArgument));
QueueHandle_t freqencyQueue   = xQueueCreate(1, sizeof(uint8_t));; 
TaskHandle_t intentFreqHandle = NULL;

SleepControlConf sleepCtrlConf = {GPIO_SEL_34 | GPIO_SEL_36 | GPIO_SEL_39, ESP_EXT1_WAKEUP_ALL_LOW};
SleepControl sleepControl(sleepCtrlConf);

ESP32Time rtc(0);
FileManager fileManager(SD, PIN_CS_SD);
TextIndex textIndex(fileManager);
DirectoryCache directoryCache(fileManager);
PowerStatus powerStatus(PIN_PWR_DET, PIN_CHG_DET, PIN_BAT_STAT);

StatusManager* statusManager  = nullptr;
AbstractIntent* intentCurrent = nullptr;


void hal_setup(void)
{
    // TODO: Change for concrete tasks, not global
    // Init and configure wdt with timeot seconds and panic flag
	esp_task_wdt_init(60, false);

    // Sleep and wake-up configs
    sleepControl.configureExt1WakeUp();
    sleepControl.setWarkupTimer(60);
    wakeUpReason = sleepControl.getWarkeupCause();
    ESP_LOGD(TAG_MAIN, "Wake Up Reason: %i", wakeUpReason);

    xTaskCreate(blink, "blinky", 4096, NULL, 5, NULL);

    // SPI ad FM
    SPI.begin();
    fileManager.begin();

    // Init lv, display joystick and file system
	lv_init();
    lv_epd_disp_init();
    lv_joystick_indev_init();
    lv_arduino_fs_init();

    // Set time on initial startup
    if (wakeUpReason == ESP_SLEEP_WAKEUP_UNDEFINED) {
        rtc.setTime(30, 15, 18, 27, 2, 2025, 0);
    }
    // Do not refresh full display ony if we're sleeping
    if (wakeUpReason != ESP_SLEEP_WAKEUP_TIMER) {
        lv_epd_mark_full();
    }

    // Status manager
    statusManager = new StatusManager(eventQueue, rtc, powerStatus);
    IntentArgument statusManagerArg(STATUS_FREQUENCY_MILLS);
    statusManager->onStartUp(statusManagerArg);

    // Build and start intent
    uint8_t startupIntentId = wakeUpReason == ESP_SLEEP_WAKEUP_TIMER ? INTENT_ID_SLEEP : INTENT_ID_HOME;
    buildIntent(startupIntentId);
    intentCurrent->onStartUp(IntentArgument::NO_ARG);
    
    // Create event queue adnd frequency tasks 
    void *queues[2] = { eventQueue, freqencyQueue };
    xTaskCreate(eventQueueTask, "uiTask", 4096 * 2, queues, 1, nullptr);
    xTaskCreate(taskIntentFreq, "intentFreq", 2048, freqencyQueue, 1, &intentFreqHandle);

    // create_black_square(lv_scr_act());
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
    uint8_t frequencyArg = 0;

    for(;;) {

        unsigned long timeSinceLastInteraction = millis() - lv_joystick_last_hit();
        ESP_LOGD(TAG_MAIN, "Mills since last interaction: %i", timeSinceLastInteraction);

        // Once timeout is reached, frequency arg stays UINT8_MAX and never reset; 
        // This is done on purpose, so user input does not interrup sleep seq
        if (timeSinceLastInteraction > SLEEP_TIMOUT_MILLS) {
            frequencyArg = UINT8_MAX;
        }
        
        if (xQueueSend(queue, &frequencyArg, pdMS_TO_TICKS(10)) != pdPASS)
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
    bool sleepInitiatedByTimeout = false;

    QueueHandle_t eventQueue = (QueueHandle_t)queues[0];
    QueueHandle_t freqencyQueue = (QueueHandle_t)queues[1];

    ActionArgument actionArgument;
    uint8_t frequencyArgment;

    while (true)
    {
        // Make sure intent is initialized
        if (intentCurrent == nullptr || statusManager == nullptr) {
            vTaskDelay(5 / portTICK_PERIOD_MS );
            continue;
        }

        // Wait to get item from frequency producer
        if (xQueueReceive(freqencyQueue, &frequencyArgment, pdMS_TO_TICKS(5)) == pdPASS) {
            ESP_LOGV(TAG_MAIN, "Executing inetent frequency task with value: %i", frequencyArgment);

            if (frequencyArgment == UINT8_MAX && !sleepInitiatedByTimeout)
            {    
                ESP_LOGD(TAG_MAIN, "Initiating sleep sequence caused by inactivity timeout");

                // Initiate sleep sequence
                // TODO: Status manager flickering because lv_epd_mark_full(); clears display buffers fix
                lv_epd_mark_full();
                switchIntent(INTENT_ID_SLEEP, IntentArgument::NO_ARG);

                // Assure single execution
                sleepInitiatedByTimeout = true;
            }

            // Account for intent init
            statusManager->onFrequncy();
            intentCurrent->onFrequncy();
        }

        // Check if actions were prformed
        if (xQueueReceive(eventQueue, &actionArgument, pdMS_TO_TICKS(5)) == pdPASS)
        {
            // ESP_LOGD(TAG_MAIN, "Received event: target=%p, code=%d", actionArg.target, actionArg.code);
            ActionResult result = intentCurrent->onAction(actionArgument);

            if (result.type == ActionRetultType::CHANGE_INTENT) {
                ESP_LOGD(TAG_MAIN, "Change intent action fired with id: %i", result.id);
                // Force full refresh on change intent
                lv_epd_mark_full();
                switchIntent(result.id, result.data);
                // Refresh status manager
                statusManager->onAction(actionArgument);
		    }
        }

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
    case INTENT_ID_BOOK:
		intentCurrent = new IntentBook(eventQueue, fileManager, textIndex, directoryCache);
		break;
    case INTENT_ID_CONF:
        intentCurrent = new IntentConf(eventQueue, rtc);
        break;
	case INTENT_ID_SLEEP:
		intentCurrent = new IntentSleep(eventQueue, sleepControl, rtc);
		break;
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

#endif