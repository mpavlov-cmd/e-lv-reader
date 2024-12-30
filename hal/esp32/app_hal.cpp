
#include "app_hal.h"
#include "lvgl.h"

#include <Arduino.h>
#include <SPI.h>

#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include "DisplayBuffer.h"
// #include "Ap_29demo.h"

static const uint32_t screenWidth = 480;
static const uint32_t screenHeight = 800;

const unsigned int lvBufferSize = screenWidth * 32;
uint8_t lvBuffer[lvBufferSize];

static lv_display_t *lvDisplay;
static lv_indev_t *lvInput;

uint16_t chunkCounter = 0;

#if LV_USE_LOG != 0
static void lv_log_print_g_cb(lv_log_level_t level, const char *buf)
{
    LV_UNUSED(level);
    LV_UNUSED(buf);
}
#endif

/* Display flushing */
// In Driver: 0X00 - Black; 0XFF: White (Every value contains 8 bits - )
// In Driver: Each next char fills pixel: X0Y0, X0Y1, X0Y2, X0Y3

// TODO: Make the damn thig work
// Example of the working configuration
// https://github.com/lvgl/lvgl_esp32_drivers/blob/master/lvgl_tft/uc8151d.c#L211
// Initial example working now
// https://github.com/lvgl/lv_platformio/blob/master/platformio.ini
// Full documentation on proting
// https://docs.lvgl.io/8/porting/display.html#examples
void my_disp_flush(lv_display_t *lvDisplay, const lv_area_t *area, unsigned char *px_map)
{
    if (chunkCounter > 0)
    {
        lv_display_flush_ready(lvDisplay);
    }
    else
    {

        Serial.println("Flush");

        int width = lv_area_get_width(area);
        int height = lv_area_get_height(area);

        lv_draw_sw_rgb565_swap(px_map, width * height);

        Serial.printf("Width and height: %ix%i\n", width, height);
        Serial.printf("Start pos: %ix%i\n", area->x1, area->y1);

        // Prepare a buffer to store the e-paper-compatible image data

        unsigned char epd_buffer[(width * height + 7) / 8];
        memset(epd_buffer, 0XFF, sizeof(epd_buffer)); // Initialize to white
        Serial.printf("Size of buffer: %i\n", sizeof(epd_buffer));

        int32_t x, y;
        for (y = area->y1; y <= area->y2; y++)
        {
            for (x = area->x1; x <= area->x2; x++)
            {

                // Calculate the index in the px_map buffer for column-first order
                // int32_t index = (x - area->x1) * height + (y - area->y1);
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
                uint8_t pixel = (luminance > 136) ? 0 : 1; // 1 for black, 0 for white

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
                // if (y < 32 && x < 32)
                // {
                //     Serial.printf("Index: %i; Pos: %i:%i; Color: %i\n", index, x, y, color);
                // }
            }
        }

        for (int i = 0; i < (width * height + 7) / 8; i++)
        {
            Serial.printf("0x%02X, ", epd_buffer[i]);
            if ((i + 1) % 8 == 0 && i != 0)
            {
                Serial.println();
            }
        }

        EPD_HW_Init();
        EPD_SetRAMValue_BaseMap(gImageBlank);
        EPD_Dis_Part(area->x1, area->y1, epd_buffer, width, height);
        EPD_DeepSleep();

        chunkCounter++;

        lv_display_flush_ready(lvDisplay); /* tell lvgl that flushing is done */
    }
}

/* Tick source, tell LVGL how much time (milliseconds) has passed */
static uint32_t my_tick(void)
{
    return millis();
}

void hal_setup(void)
{

    Serial.begin(115200);

    // Set display pins
    pinMode(4, INPUT);   // BUSY
    pinMode(16, OUTPUT); // RES
    pinMode(17, OUTPUT); // DC
    pinMode(SS, OUTPUT); // CS

    // SPI
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
    SPI.begin();

    EPD_HW_Init();           // Full screen refresh initialization.
    EPD_WhiteScreen_White(); // Clear screen function.
    EPD_DeepSleep();         // Enter the sleep mode and please do not delete it, otherwise it will reduce the lifespan of the screen.
    delay(2000);             // Delay for 2s.

    /* Set the tick callback */
    lv_tick_set_cb(my_tick);

    /* Create LVGL display and set the flush function */
    lvDisplay = lv_display_create(screenWidth, screenHeight);
    lv_display_set_color_format(lvDisplay, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(lvDisplay, my_disp_flush);
    lv_display_set_buffers(lvDisplay, lvBuffer, NULL, lvBufferSize, LV_DISPLAY_RENDER_MODE_PARTIAL);

    // TODO: initialize input method
    /* Set the touch input function */
    // lvInput = lv_indev_create();
    // lv_indev_set_type(lvInput, LV_INDEV_TYPE_POINTER);
    // lv_indev_set_read_cb(lvInput, my_touchpad_read);
}

void hal_loop(void)
{
    /* NO while loop in this function! (handled by framework) */
    lv_timer_handler(); // Update the UI-
    delay(5);
}