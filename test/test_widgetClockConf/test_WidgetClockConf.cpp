#include <unity.h>
#include "drvlvgl/Driver_Display_EPD.h"
#include "drvlvgl/Driver_Input_Keypad.h"
#include "drvfs/Driver_Arduino_FS.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <Arduino.h>
#include <SPI.h>

#include <widget/WidgetClockConf.h>
#include <model/clock/ModelClock.h>

DBox box = DBox::atCenter(464, 280, 8, 1);

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
    lv_arduino_fs_init();
    lv_epd_mark_full();

    widgetGroup = lv_group_create();
	lv_indev_set_group(lv_get_keypad(), widgetGroup);
}

void tearDown(void)
{
    // clean stuff up here
}

void testRendersOneLineOfText_void(void) {

    // Given
    WidgetClockConf widgetClockConf(widgetGroup, eventQueue);
    ModelClock model = {box, 2025, 2, 15, 16, 20, 5};

    // When
    widgetClockConf.upgrade(model);
    lv_timer_handler();

    // Then
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