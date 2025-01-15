#include <unity.h>
#include "drvlvgl/Driver_Display_EPD.h"
#include "drvlvgl/Driver_Input_Keypad.h"
#include "drvfs/Driver_Arduino_FS.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <set/Set.h>
#include <FileManager.h>
#include <ButtonActions.h>
#include <SleepControl.h>

#include <widget/WidgetStatus.h>

DBox box = DBox::atCenter(480, 32, 8, 1);

lv_group_t* widgetGroup;
QueueHandle_t eventQueue = xQueueCreate(256, sizeof(ActionArgument));

void setUp(void)
{   
    Serial.begin(115200);

    SPI.begin();

    // Initalize display as it is used by text index
    lv_init();
    lv_epd_disp_init();
    lv_joystick_indev_init();

    // Init lv file system 
    lv_arduino_fs_init();

    widgetGroup = lv_group_create();
	lv_indev_set_group(lv_get_keypad(), widgetGroup);
}

void tearDown(void)
{
    // clean stuff up here
}

void testRendersOneLineOfText_void(void) {

    WidgetStatus* widgetStatus;
    
    // Given 
    // Unplagged, 3%
    ModelStatus model = {box, false, false, 3, "00:00", "", &lv_font_montserrat_18};
    widgetStatus = new WidgetStatus(widgetGroup, eventQueue);

    // When
    widgetStatus->upgrade(model);
    lv_timer_handler();

    // Then
    delete widgetStatus;
    delay(1000);

    // ----------------------

    // Given
    // Plugged, chargeing
    widgetStatus = new WidgetStatus(widgetGroup, eventQueue);
    model.plugged = true;
    model.charging = true;
    strncpy(model.time, "08:30", sizeof(model.time));

    // When
    widgetStatus->upgrade(model);
    lv_timer_handler();

    // Then
    delete widgetStatus;
    delay(1000);

    // ----------------------

    // Given
    // Plugged charge complete
    widgetStatus = new WidgetStatus(widgetGroup, eventQueue);
    model.plugged = true;
    model.charging = false;
    strncpy(model.time, "14:00", sizeof(model.time));
    strncpy(model.extra, LV_SYMBOL_WARNING, sizeof(model.extra));

    // When
    widgetStatus->upgrade(model);
    lv_timer_handler();

    // Then
    delete widgetStatus;
    delay(1000);

    // ----------------------

    // Given
    // Unplagged, 100%
    widgetStatus = new WidgetStatus(widgetGroup, eventQueue);
    model.plugged  = false;
    model.charging = false;
    model.batteryLevel = 100;
    strncpy(model.time, "23:59", sizeof(model.time));
    strncpy(model.extra, LV_SYMBOL_ENVELOPE, sizeof(model.extra));

    // When
    widgetStatus->upgrade(model);
    lv_timer_handler();

    // Then
    delete widgetStatus;
    delay(1000);

    TEST_ASSERT_TRUE(true);
}

// Actual test runner
void setup()
{
    delay(2000); // service delay
    UNITY_BEGIN();

    RUN_TEST(testRendersOneLineOfText_void);

    UNITY_END(); // stop unit testing
}

void loop()
{
}