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

#include <widget/WidgetBookStat.h>

// DBox box{0, 48, 480, 704, 8, 1};
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

    // Given
    WidgetBookStat* bookStat;
    
    ModelBookStat model = {box, 51, 100, &lv_font_montserrat_14};
    bookStat = new WidgetBookStat(widgetGroup, eventQueue);

    // When
    bookStat->upgrade(model);
    lv_timer_handler();

    // Then
    delete bookStat;
    delay(1000);

    // ------------------------

    // Given
    model.currentPage = 0;
    bookStat = new WidgetBookStat(widgetGroup, eventQueue);

    // When
    bookStat->upgrade(model);
    lv_timer_handler();

    // Then
    delete bookStat;
    delay(1000);

    // ------------------------

    // Given
    model.currentPage = 100;
    bookStat = new WidgetBookStat(widgetGroup, eventQueue);

    // When
    bookStat->upgrade(model);
    lv_timer_handler();

    // Then
    delete bookStat;

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