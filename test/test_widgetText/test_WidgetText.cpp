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

DBox textBox{24, 48, 432, 704, 1, 1};
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
    WidgetText widgetText(widgetGroup, eventQueue);
   
    ModelText* modelText = ModelText::newPtr();
    modelText->box   = textBox;
    modelText->text  = "Single line text";
    modelText->align = LV_ALIGN_CENTER;
    modelText->font  = &lv_font_montserrat_24; 

    // When
    widgetText.upgrade(*modelText);
    lv_timer_handler();

    // Then
    TEST_ASSERT_TRUE(true);

    // Cleanup 
    delete modelText;
}

void testRendersMultileLinesOfText_void(void) {

    // Given
    String text = "This is multi-line text which should fit into the screen.\n";
    text += "Second line goes here, it is approxumately the lenght as 1\n";
    text += "and this is the third and the final line, looks good Yes?\n";
    text += "\n";
    text += "this line goes after double line break, see if it is good!";

    WidgetText widgetText(widgetGroup, eventQueue);

    ModelText* modelText = ModelText::newPtr();
    modelText->box = textBox;
    modelText->text = text;

    // When
    widgetText.upgrade(*modelText);
    lv_timer_handler();

    // Then
    TEST_ASSERT_TRUE(true);
    
    // Cleanup 
    delete modelText;
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