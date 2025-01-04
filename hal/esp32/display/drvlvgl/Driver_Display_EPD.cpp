#include "Driver_Display_EPD.h"

#include <Arduino.h>
#include "lvgl.h"
#include "LogTags.h"
#include "esp_log.h"

#include "PinDefinitions.h"
#include "drvspi/Display_EPD_W21_spi.h"
#include "drvspi/Display_EPD_W21.h"
#include "drvspi/Display_EPD_Buffer.h"

// Function definitions
void epd_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void epd_rounder_cb(lv_disp_drv_t * disp_drv, lv_area_t * area);
void tick_timer_callback(void *arg);

#ifndef MY_DISP_HOR_RES
#define MY_DISP_HOR_RES 480
#endif

#ifndef MY_DISP_VER_RES
#define MY_DISP_VER_RES 800
#endif

// Display variables
// Static or global buffer(s). The second buffer is optional
static lv_disp_draw_buf_t draw_buf_dsc_1;
static lv_color_t buf_1[MY_DISP_HOR_RES * 32];

static lv_disp_drv_t disp_drv; 
static lv_disp_t* disp;   

// TODO: Figure out why moving this counter causes MCU boot failure
uint16_t chunkCounter = 0;

/**
 * Requires SPI to be initialized 
 */
void lv_epd_disp_init(void)
{
    // Set display pins
    pinMode(PIN_DISP_BUSY, INPUT);
    pinMode(PIN_DISP_RST,  OUTPUT);
    pinMode(PIN_DISP_DC,   OUTPUT); 
    pinMode(PIN_DSIP_CS,   OUTPUT);

    // Full screen refresh initialization.
    EPD_HW_Init_Fast();          
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
    disp_drv.rounder_cb = epd_rounder_cb;
    disp_drv.draw_buf = &draw_buf_dsc_1;

    // Get pointer to the active display 
    disp = lv_disp_drv_register(&disp_drv);

    // Lunch tick timer for display refreh
    // Set up the tick timer
    const esp_timer_create_args_t tick_timer_args = {
        .callback = &tick_timer_callback,
        .name = "lv_tick_timer"
    };
    esp_timer_handle_t tick_timer;
    esp_timer_create(&tick_timer_args, &tick_timer);
    esp_timer_start_periodic(tick_timer, 1000); // 1ms interval
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
    ESP_LOGD(TAG_DISPL, "--- Flush Start ---");

    int width = lv_area_get_width(area);
    int height = lv_area_get_height(area);

    //  lv_draw_sw_rgb565_swap(px_map, width * height);

    ESP_LOGD(TAG_DISPL, "Width and height: %ix%i", width, height);
    ESP_LOGD(TAG_DISPL, "Start pos: %ix%i", area->x1, area->y1);

    // Prepare a buffer to store the e-paper-compatible image data
    unsigned char epd_buffer[(width * height) / 8];
    memset(epd_buffer, 0XFF, sizeof(epd_buffer)); // Initialize to white
    ESP_LOGD(TAG_DISPL, "Size of buffer: %i", sizeof(epd_buffer));

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
    //         ESP_LOGD(TAG_DISPL, "0x%02X, ", epd_buffer[i]);
    //         if ((i + 1) % 8 == 0 && i != 0)
    //         {
    //              ESP_LOGD(TAG_DISPL, "")
    //         }
    //     }
    // }

    ESP_LOGD(TAG_DISPL, "--- Before RAM ---");
    if (chunkCounter == 0)
    {
        ESP_LOGD(TAG_DISPL, "--- Initial Update ---");
    //     EPD_HW_Init_Fast();
    //     EPD_SetRAMValue_BaseMap(gImageBlank);
    }

    ESP_LOGD(TAG_DISPL, "--- x1,y1: %ix%i width: %i, height: %i ---", area->x1, area->y1, width, height);
    EPD_Dis_Part_RAM(area->y1, area->x1, epd_buffer, width, height);
    chunkCounter++;

    // if (chunkCounter == 10) {
    if (lv_disp_flush_is_last(disp_drv)) {
        chunkCounter = 0;
        EPD_Part_Update();
        EPD_DeepSleep();
        ESP_LOGD(TAG_DISPL, "--- Flush completed ---");
    }
    
    // Anyway tell lgvl that display is ready
    lv_disp_flush_ready(disp_drv); /* tell lvgl that flushing is done */
}

void epd_rounder_cb(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    // Round the x1 coordinate down to the nearest multiple of 8
    area->x1 = (area->x1 / 8) * 8;
    // Round the x2 coordinate up to the nearest multiple of 8
    area->x2 = ((area->x2 + 7) / 8) * 8 - 1;
    // Round the y1 coordinate down to the nearest multiple of 8
    area->y1 = (area->y1 / 8) * 8;
    // Round the y2 coordinate up to the nearest multiple of 8
    area->y2 = ((area->y2 + 7) / 8) * 8 - 1;
}

void tick_timer_callback(void *arg) {
    // Increment LVGL tick by 1 ms
    lv_tick_inc(1); 
}