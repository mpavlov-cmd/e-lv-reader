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

#include <widget/WidgetText.h>

DBox textBox{24, 48, 432, 704, 2, 0};
lv_group_t * widgetGroup;
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
	lv_indev_set_group(get_lv_keypad(), widgetGroup);
}

void tearDown(void)
{
    // clean stuff up here
}

void testRendersOneLineOfText_void(void) {

    // Given
    WidgetText widgetText(widgetGroup, eventQueue);
    ModelText modelText{textBox, "Single line text"};

    // When
    widgetText.upgrade(modelText);
    lv_timer_handler();

    // Then
    TEST_ASSERT_TRUE(true);
}

void testRendersMultileLinesOfText_void(void) {

    // Given
    String text = "This is multi-line text which should fit into the screen.\n";
    text += "Second line goes here, it is approxumately the lenght as 1\n";
    text += "and this is the third and the final line, looks good Yes?\n";
    text += "\n";
    text += "this line goes after double line break, see if it is good!";

    WidgetText widgetText(widgetGroup, eventQueue);
    ModelText modelText{textBox, text};

    // When
    widgetText.upgrade(modelText);
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
    RUN_TEST(testRendersMultileLinesOfText_void);

    UNITY_END(); // stop unit testing
}

void loop()
{
}