
#include "app_hal.h"
#include "lvgl.h"

#include <Arduino.h>
#include <SPI.h>

#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "DisplayBuffer.h"

#include "PinDefinitions.h"
#include "SwithInputHandler.h"
#include "ButtonActions.h"

// Function definitions
void epd_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void joystick_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
void IRAM_ATTR isr();

#if LV_USE_LOG != 0
static void lv_log_print_g_cb(lv_log_level_t level, const char *buf)
{
    LV_UNUSED(level);
    LV_UNUSED(buf);
}
#endif

#ifndef MY_DISP_HOR_RES
    #define MY_DISP_HOR_RES 480
#endif

#ifndef MY_DISP_VER_RES
    #define MY_DISP_VER_RES 800
#endif

// #define LV_TICK_CUSTOM 1
// #define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

// Display variables
// Static or global buffer(s). The second buffer is optional
static lv_disp_draw_buf_t draw_buf_dsc_1;
static lv_color_t buf_1[MY_DISP_HOR_RES * 32];

static lv_disp_drv_t disp_drv; 
static lv_disp_t* disp;   

// TODO: Figure out why moving this counter causes MCU boot failure
uint16_t chunkCounter = 0;

// Input control
static lv_indev_drv_t indev_drv;
static lv_indev_t * indev_keypad;

SwithInputHandler inputHandler(BT_INPUT_2, BT_INPUT_1, BT_INPUT_0);
volatile bool isrPending = false; 
volatile unsigned long isrStartedAt = 0;

void tick_timer_callback(void *arg) {
    lv_tick_inc(1); // Increment LVGL tick by 1 ms
}

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

static void event_handler(lv_event_t * e) {
    Serial.println("Button Event!");
    create_black_square(lv_scr_act());
    // lv_event_code_t code = lv_event_get_code(e);
    // lv_obj_t * obj = lv_event_get_target(e);
    // if(code == LV_EVENT_CLICKED) {
    // }
}

void lv_example_list_1(void)
{
    lv_group_t * widget_group = lv_group_create();

    /*Create a list*/
    lv_obj_t * list1 = lv_list_create(lv_scr_act());
    lv_obj_set_size(list1, 256, 512);
    lv_obj_center(list1);

    /*Add buttons to the list*/
    lv_list_add_text(list1, "File");

    lv_obj_t * btn1 = lv_list_add_btn(list1, LV_SYMBOL_FILE, "New");
    lv_group_add_obj(widget_group, btn1);
    lv_obj_add_state(btn1, LV_STATE_CHECKED);
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t * btn2 = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, "Open");
    lv_group_add_obj(widget_group, btn2);
    lv_obj_add_event_cb(btn2, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t * btn3 = lv_list_add_btn(list1, LV_SYMBOL_SAVE, "Save");
    lv_group_add_obj(widget_group, btn3);
    lv_obj_add_event_cb(btn3, event_handler, LV_EVENT_CLICKED, NULL);

	static lv_style_t style_font24;
    lv_style_init(&style_font24);

    // Set the text font size
    lv_style_set_text_font(&style_font24, &lv_font_montserrat_24); 

	lv_obj_add_style(list1, &style_font24, LV_PART_MAIN); // Apply to the main part
	lv_obj_add_style(list1, &style_font24, LV_PART_ITEMS);

    // Button control 
    lv_indev_set_group(indev_keypad, widget_group);
}

void hal_setup(void)
{
    // ----- EPD INIT BEGIN -----
    // Set display pins
    pinMode(4, INPUT);   // BUSY
    pinMode(16, OUTPUT); // RES
    pinMode(17, OUTPUT); // DC
    pinMode(SS, OUTPUT); // CS

    // SPI
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    SPI.begin();

    EPD_HW_Init_Fast();           // Full screen refresh initialization.
    // Set buffer for patia refresh
    EPD_SetRAMValue_BaseMap(gImageBlank);

    // EPD_WhiteScreen_White(); // Clear screen function.
    // EPD_DeepSleep();         // Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    // delay(2000);             // Delay for 2s.

    // ----- EPD INIT END -----
    // ----- DISPLAY DRIVER INIT START -----

    // ------ Start display config ------
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 32);   /*Initialize the display buffer*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    // Set the resolution of the display
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    // Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = epd_flush_cb;
    // Set a display buffer
    disp_drv.draw_buf = &draw_buf_dsc_1;

    // Get pointer to the active display 
    disp = lv_disp_drv_register(&disp_drv);
    // ----- DISPLAY DRIVER INIT END -----
    // ----- INPUT DRIVER INIT START -----

    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = joystick_read;
    indev_keypad      = lv_indev_drv_register(&indev_drv);

    inputHandler.configure(isr, 100, 2500);

    // Set up the tick timer
    const esp_timer_create_args_t tick_timer_args = {
        .callback = &tick_timer_callback,
        .name = "lv_tick_timer"
    };
    esp_timer_handle_t tick_timer;
    esp_timer_create(&tick_timer_args, &tick_timer);
    esp_timer_start_periodic(tick_timer, 1000); // 1ms interval

    // ----- INPUT DRIVER INIT END -----
    // create_black_square(lv_disp_get_scr_act(disp));
    lv_example_list_1();
}

void hal_loop(void)
{
    // Update the UI
    lv_timer_handler(); 
    delay(5);
}

// Display flushing 
// In Driver: 0X00 - Black; 0XFF: White (Every value contains 8 bits)
// In Driver: Each next char fills pixel: X0Y0, X0Y1, X0Y2, X0Y3

// Example of the working configuration
// https://github.com/lvgl/lvgl_esp32_drivers/blob/master/lvgl_tft/uc8151d.c#L211
// Initial example working now
// https://github.com/lvgl/lv_platformio/blob/master/platformio.ini
// Full documentation on proting
// https://docs.lvgl.io/8/porting/display.html#examples
void epd_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    Serial.println("Flush start");

    int width = lv_area_get_width(area);
    int height = lv_area_get_height(area);

    //  lv_draw_sw_rgb565_swap(px_map, width * height);

    Serial.printf("Width and height: %ix%i\n", width, height);
    Serial.printf("Start pos: %ix%i\n", area->x1, area->y1);

    // Prepare a buffer to store the e-paper-compatible image data
    unsigned char epd_buffer[(width * height) / 8];
    memset(epd_buffer, 0XFF, sizeof(epd_buffer)); // Initialize to white
    Serial.printf("Size of buffer: %i\n", sizeof(epd_buffer));

    int32_t x, y;
    for (y = area->y1; y <= area->y2; y++)
    {
        for (x = area->x1; x <= area->x2; x++)
        {
            // Calculate the index in the px_map buffer for column-first order
            int32_t index = (y - area->y1) * width + (x - area->x1);

            // Retrieve the pixel color as uint16_t (assuming px_map is RGB565)
            lv_color_t currentColor = ((lv_color_t *)color_p)[index];

            // Threshold to determine black or white
            uint8_t pixel = (lv_color_brightness(currentColor) > 128) ? 0 : 1; // 1 for black, 0 for white

            // Calculate the position in the buffer for column-major drawing
            int bufIdx = (x - area->x1) * height + (y - area->y1);
            int bufIdxCmp = bufIdx / 8;     // Byte index
            uint8_t bufIdxPos = bufIdx % 8; // Bit position within the byte

            // Set the pixel in the buffer (monochrome format)
            if (pixel)
            {
                epd_buffer[bufIdxCmp] &= ~(1 << (7 - bufIdxPos)); // Set black pixel
            }
            else
            {
                epd_buffer[bufIdxCmp] |= (1 << (7 - bufIdxPos)); // Set white pixel
            }
        }
    }

    // Print for debug purposes
    // if (chunkCounter < 4)
    // {
    //     for (int i = 0; i < (width * height + 7) / 8; i++)
    //     {
    //         Serial.printf("0x%02X, ", epd_buffer[i]);
    //         if ((i + 1) % 8 == 0 && i != 0)
    //         {
    //             Serial.println();
    //         }
    //     }
    // }

    Serial.println("Before RAM");
    if (chunkCounter == 0)
    {
        Serial.println("-------------------------- Initial Update --------------------------");
        // EPD_HW_Init_Fast();
        // EPD_SetRAMValue_BaseMap(gImageBlank);
    }

    Serial.printf("------------ x1,y1: %ix%i width: %i, height: %i ------------\n", area->x1, area->y1, width, height);
    EPD_Dis_Part_RAM(area->y1, area->x1, epd_buffer, width, height);
    chunkCounter++;

    // if (chunkCounter == 10) {
    if (lv_disp_flush_is_last(disp_drv)) {
        chunkCounter = 0;
        EPD_Part_Update();
        EPD_DeepSleep();
    }
    
    // Anyway tell lgvl that display is ready
    lv_disp_flush_ready(disp_drv); /* tell lvgl that flushing is done */
}


// LV_KEY_UP        = 17,  /*0x11*/
// LV_KEY_DOWN      = 18,  /*0x12*/
// LV_KEY_RIGHT     = 19,  /*0x13*/
// LV_KEY_LEFT      = 20,  /*0x14*/
// LV_KEY_ESC       = 27,  /*0x1B*/
// LV_KEY_DEL       = 127, /*0x7F*/
// LV_KEY_BACKSPACE = 8,   /*0x08*/
// LV_KEY_ENTER     = 10,  /*0x0A, '\n'*/
// LV_KEY_NEXT      = 9,   /*0x09, '\t'*/
// LV_KEY_PREV      = 11,  /*0x0B, '*/
// LV_KEY_HOME      = 2,   /*0x02, STX*/
// LV_KEY_END       = 3,   /*0x03, ETX*/

void joystick_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static bool btnPressed = false;
    bool heldBit = false;
    
    // Serial.println("Joystick read triggered");
    uint8_t switchInput = inputHandler.handleInput(isrPending, isrStartedAt);
    if (switchInput) {
        Serial.println("Button Pressed");

        // Mark btn as pressed 
        data->state = LV_INDEV_STATE_PRESSED;
        btnPressed  = true;

        const uint8_t directionInput = controlDirection(switchInput, heldBit);

        if (directionInput == BUTTON_ACTION_LEFT) {
            data->key = LV_KEY_LEFT;
        }

        if (directionInput == BUTTON_ACTION_RIGHT) {
            data->key = LV_KEY_RIGHT;
        }

        if (directionInput == BUTTON_ACTION_UP) {
             data->key = LV_KEY_UP;
        }

        if (directionInput == BUTTON_ACTION_DOWN) {
            data->key = LV_KEY_DOWN;
        }

        if (directionInput == BUTTON_ACTION_MID) {
             data->key = LV_KEY_ENTER;
        }

    } else {
        // TODO: Figure out wy released is triggered while held
        if (btnPressed) {
            Serial.println("Button Released");

            btnPressed  = false; 
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }
}

void IRAM_ATTR isr()
{
    if (isrPending) {
		return;
	}

	isrStartedAt = millis();
	isrPending = true;
}
