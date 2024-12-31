
#include "app_hal.h"
#include "lvgl.h"

#include <Arduino.h>
#include <SPI.h>

#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "DisplayBuffer.h"

#include "PinDefinitions.h"
#include "SwithInputHandler.h"

// Function definitions
static uint32_t arduino_tick(void);
void epd_flush_cb(lv_display_t *lvDisplay, const lv_area_t *area, unsigned char *px_map);

void joystick_read(lv_indev_t * indev, lv_indev_data_t * data);
void IRAM_ATTR isr();

#if LV_USE_LOG != 0
static void lv_log_print_g_cb(lv_log_level_t level, const char *buf)
{
    LV_UNUSED(level);
    LV_UNUSED(buf);
}
#endif

// Display variables
static const uint32_t screenWidth  = 480;
static const uint32_t screenHeight = 800;
const unsigned int lvBufferSize = screenWidth * 32;
uint8_t lvBuffer[lvBufferSize];

static lv_display_t *lvDisplay;

// TODO: Figure out why moving this counter causes MCU boot failure
uint16_t chunkCounter = 0;


// Input control
SwithInputHandler inputHandler(BT_INPUT_2, BT_INPUT_1, BT_INPUT_0);
volatile bool isrPending = false; 
volatile unsigned long isrStartedAt = 0;

static lv_indev_t *lvInput;

void hal_setup(void)
{

    // Set display pins
    pinMode(4, INPUT);   // BUSY
    pinMode(16, OUTPUT); // RES
    pinMode(17, OUTPUT); // DC
    pinMode(SS, OUTPUT); // CS

    // SPI
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    SPI.begin();

    EPD_HW_Init_Fast();           // Full screen refresh initialization.
    // EPD_WhiteScreen_White(); // Clear screen function.
    // EPD_DeepSleep();         // Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    // delay(2000);             // Delay for 2s.

    /* Set the tick callback */
    lv_tick_set_cb(arduino_tick);

    /* Create LVGL display and set the flush function */
    lvDisplay = lv_display_create(screenWidth, screenHeight);
    lv_display_set_color_format(lvDisplay, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(lvDisplay, epd_flush_cb);
    lv_display_set_buffers(lvDisplay, lvBuffer, NULL, lvBufferSize, LV_DISPLAY_RENDER_MODE_PARTIAL);

    // Set the input function
    inputHandler.configure(isr, 100, 2500);

    lvInput = lv_indev_create();
    lv_indev_set_type(lvInput, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(lvInput, joystick_read);
}

void hal_loop(void)
{
    // Update the UI
    lv_timer_handler(); 
    delay(5);
}

// static lv_indev_t *lvInput;
// Tick source, tell LVGL how much time (milliseconds) has passed
static uint32_t arduino_tick(void)
{
    return millis();
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
void epd_flush_cb(lv_display_t *lvDisplay, const lv_area_t *area, unsigned char *px_map)
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
            uint16_t color = ((uint16_t *)px_map)[index];

            // Extract RGB components from the RGB565 color
            uint8_t r = (color >> 11) & 0x1F; // Red component (5 bits)
            uint8_t g = (color >> 5) & 0x3F;  // Green component (6 bits)
            uint8_t b = color & 0x1F;         // Blue component (5 bits)

            // Scale RGB components to 8 bits
            r = (r * 255) / 31;
            g = (g * 255) / 63;
            b = (b * 255) / 31;

            // Calculate luminance using the formula
            uint8_t luminance = (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);
            // Threshold to determine black or white
            uint8_t pixel = (luminance > 128) ? 0 : 1; // 1 for black, 0 for white

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

    if (chunkCounter == 0)
    {
        // Serial.println("-------------------------- Initial Update --------------------------");
        // EPD_HW_Init_Fast();
        EPD_SetRAMValue_BaseMap(gImageBlank);
    }


    Serial.printf("------------ x1,y1: %ix%i width: %i, height: %i ------------\n", area->x1, area->y1, width, height);
    EPD_Dis_Part_RAM(area->y1, area->x1, epd_buffer, width, height);
    chunkCounter++;

    // if (chunkCounter == 10) {
    if (lv_disp_flush_is_last(lvDisplay)) {
        chunkCounter = 0;
        EPD_Part_Update();
        EPD_DeepSleep();
    }
    
    // Anyway tell lgvl that display is ready
    lv_display_flush_ready(lvDisplay); /* tell lvgl that flushing is done */
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

void joystick_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    // Serial.println("Joystick read triggered");
    uint8_t switchInput = inputHandler.handleInput(isrPending, isrStartedAt);
    if (switchInput) {
        Serial.println("Joystick read triggered");
    }
    // data->key = last_key(); /* Get the last pressed or released key */

    // if (key_pressed()) {
    //     data->state = LV_INDEV_STATE_PRESSED;
    // }
    // else {
    //     data->state = LV_INDEV_STATE_RELEASED;
    // } 
}

void IRAM_ATTR isr()
{
    if (isrPending) {
		return;
	}

	isrStartedAt = millis();
	isrPending = true;
}
