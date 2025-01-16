#include "Driver_Display_EPD.h"

#include <Arduino.h>
#include <SPI.h>
#include "lvgl.h"
#include "LogTags.h"
#include "esp_log.h"

#include "PinDefinitions.h"
#include "drvspi/Display_EPD_W21_spi.h"
#include "drvspi/Display_EPD_W21.h"

// Strct definiton
struct BufferData {
    unsigned char* buffer;  // Pointer to the buffer
    int x;                  // x-coordinate
    int y;                  // y-coordinate
    int width;              // Width of the buffer
    int height;             // Height of the buffer
};

// Function definitions
void epd_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void epd_rounder_cb(lv_disp_drv_t * disp_drv, lv_area_t * area);

#ifdef EPD_FLUSH_REWORK
void epd_flush_cb_new(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void epd_set_px_cb_new(struct _lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
    lv_color_t color, lv_opa_t opa);
#endif

void tick_timer_callback(void *arg);

#define MY_DISP_MAX_BUFFERS 30

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
// static lv_color_t buf_2[MY_DISP_HOR_RES * 32];

static lv_disp_drv_t disp_drv; 
static lv_disp_t* disp;   

// TODO: Figure out why moving this counter causes MCU boot failure
uint16_t chunkCounter = 0;
bool forceDispRefresh = false;

static BufferData* bufferCollection = nullptr;
int bufferCount = 0; // To track the number of stored buffers

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

    // Full screen refresh initialization and set buffer for patial refresh should be triggered by 
    // lv_epd_mark_full during first display init or in tests

    // EPD_HW_Init_Fast();          
    // EPD_SetRAMValue_Empty_BaseMap();

    // ----- DISPLAY DRIVER INIT START -----

    // ------ Start display config ------
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, NULL, MY_DISP_HOR_RES * 32);  
    lv_disp_drv_init(&disp_drv);                   

    /*Set up the functions to access to your display*/

    // Set the resolution of the display
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;

    // Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = epd_flush_cb;
    disp_drv.rounder_cb = epd_rounder_cb;
    // disp_drv.set_px_cb = epd_set_px_cb_new;

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

void lv_epd_mark_full(void)
{
    forceDispRefresh = true;
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
    ESP_LOGV(TAG_DISPL, "--- Flush Start ---");
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

    int width = lv_area_get_width(area);
    int height = lv_area_get_height(area);

    //  lv_draw_sw_rgb565_swap(px_map, width * height);

    ESP_LOGV(TAG_DISPL, "Width and height: %ix%i", width, height);
    ESP_LOGV(TAG_DISPL, "Start pos: %ix%i", area->x1, area->y1);

    // Prepare a buffer to store the e-paper-compatible image data
    unsigned char* epd_buffer = new unsigned char[(width * height) / 8];
    memset(epd_buffer, 0XFF, sizeof(epd_buffer)); // Initialize to white
    ESP_LOGV(TAG_DISPL, "Size of buffer: %i", sizeof(epd_buffer));

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

    ESP_LOGV(TAG_DISPL, "--- Before RAM ---");
    if (chunkCounter == 0)
    {
        ESP_LOGD(TAG_DISPL, "--- Initial Update ---");
        if (forceDispRefresh) {
            EPD_HW_Init_Fast();
            EPD_SetRAMValue_Empty_BaseMap();
            forceDispRefresh = false;
        }

        bufferCollection = new BufferData[MY_DISP_MAX_BUFFERS];
    }

    ESP_LOGV(TAG_DISPL, "--- x1,y1: %ix%i width: %i, height: %i ---", area->x1, area->y1, width, height);
    EPD_Dis_Part_RAM(0x24, area->y1, area->x1, epd_buffer, width, height);
    chunkCounter++;

    // Add buffer to collection:
    bufferCollection[bufferCount++] = {epd_buffer, area->x1, area->y1, width, height};
    ESP_LOGV(TAG_DISPL, "Added buffer to collection: %i", bufferCount);

    if (lv_disp_flush_is_last(disp_drv)) {

        // Update display with already stored display data
        EPD_Part_Update();

        // Fill buffers for next refresh
        for (int i = 0; i < bufferCount; i++) {
            ESP_LOGV(TAG_DISPL, "Writhing stored buffer data forthe next operation: %i", i);

            BufferData cbd = bufferCollection[i];
            EPD_Dis_Part_RAM(0x26, cbd.y, cbd.x, cbd.buffer, cbd.width, cbd.height);
            EPD_Dis_Part_RAM(0x24, cbd.y, cbd.x, cbd.buffer, cbd.width, cbd.height);

            ESP_LOGV(TAG_DISPL, "Deleting buffer from collection: %i", bufferCount);
            delete[] bufferCollection[i].buffer;
        }

        delete[] bufferCollection;
        bufferCollection = nullptr;
        chunkCounter = 0;
        bufferCount  = 0;

        EPD_DeepSleep();
        ESP_LOGD(TAG_DISPL, "--- Flush completed ---");
    }
    
    // Anyway tell lgvl that display is ready
    SPI.endTransaction();
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

#ifdef EPD_FLUSH_REWORK
void epd_flush_cb_new(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    ESP_LOGD(TAG_DISPL, "--- Flush Start ---");
    SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));

    int width = lv_area_get_width(area);
    int height = lv_area_get_height(area);

    //  lv_draw_sw_rgb565_swap(px_map, width * height);

    ESP_LOGD(TAG_DISPL, "Width and height: %ix%i", width, height);
    ESP_LOGD(TAG_DISPL, "Start pos: %ix%i", area->x1, area->y1);

    unsigned char* testBuf = (unsigned char*) color_p;

    // Print for debug purposes
    // if (chunkCounter < 2)
    // {
    //     for (int i = 0; i < (width * height + 7) / 8; i++)
    //     {
    //         Serial.printf("0x%02X, ", testBuf[i]);
    //         if ((i + 1) % 8 == 0 && i != 0)
    //         {
    //              Serial.println();
    //         }
    //     }
    // }

    ESP_LOGD(TAG_DISPL, "--- Before RAM ---");
    if (chunkCounter == 0)
    {
        ESP_LOGD(TAG_DISPL, "--- Initial Update ---");
        if (forceDispRefresh) {
            EPD_HW_Init_Fast();
            EPD_SetRAMValue_Empty_BaseMap();
            forceDispRefresh = false;
        }
    }

    ESP_LOGD(TAG_DISPL, "--- x1,y1: %ix%i width: %i, height: %i ---", area->x1, area->y1, width, height);
    EPD_Dis_Part_RAM(0x24, area->y1, area->x1, testBuf, width, height);
    chunkCounter++;

    // if (chunkCounter == 10) {
    if (lv_disp_flush_is_last(disp_drv)) {
        chunkCounter = 0;
        EPD_Part_Update();
        EPD_DeepSleep();
        ESP_LOGD(TAG_DISPL, "--- Flush completed ---");
    }
    
    // Anyway tell lgvl that display is ready
    SPI.endTransaction();
    lv_disp_flush_ready(disp_drv); /* tell lvgl that flushing is done */
}

void epd_set_px_cb_new(_lv_disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
    lv_color_t color, lv_opa_t opa)
{

    bool px = lv_color_brightness(color) <= 128;
    if (px) {
        Serial.printf("Found color at x: %i, y: %i\n", x, y);
    }
    
    // Threshold to determine black or white
    uint8_t pixel = (lv_color_brightness(color) > 128) ? 0 : 1; // 1 for black, 0 for white

    // Calculate the position in the buffer for column-major drawing
    int bufIdx = (x * EPD_HEIGHT + y);
    int bufIdxCmp = bufIdx / 8;     // Byte index
    uint8_t bufIdxPos = bufIdx % 8; // Bit position within the byte

    // Set the pixel in the buffer (monochrome format)
    if (pixel)
    {
        buf[bufIdxCmp] &= ~(1 << (7 - bufIdxPos)); // Set black pixel
    }

    else
    {
        buf[bufIdxCmp] |= (1 << (7 - bufIdxPos)); // Set white pixel
    }
}
#endif
