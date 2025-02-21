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

// void epd_flush_cb_new(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
void epd_set_px_cb_new(struct _lv_disp_drv_t * disp_drv, uint8_t * buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
    lv_color_t color, lv_opa_t opa);

void tick_timer_callback(void *arg);

#define MY_DISP_MAX_BUFFERS 35

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
static lv_color_t buf_2[MY_DISP_HOR_RES * 32];

static lv_disp_drv_t disp_drv; 
static lv_disp_t* disp;   

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
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf_1, buf_2, MY_DISP_HOR_RES * 32);  
    lv_disp_drv_init(&disp_drv);                   

    /*Set up the functions to access to your display*/

    // Set the resolution of the display
    disp_drv.hor_res = MY_DISP_HOR_RES;
    disp_drv.ver_res = MY_DISP_VER_RES;
    // disp_drv.direct_mode = 1;

    // Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = epd_flush_cb;
    disp_drv.rounder_cb = epd_rounder_cb;
    disp_drv.set_px_cb = epd_set_px_cb_new;

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

    ESP_LOGV(TAG_DISPL, "Width and height: %ix%i", width, height);
    ESP_LOGV(TAG_DISPL, "Start pos: %ix%i", area->x1, area->y1);

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

    EPD_Dis_Part_RAM(0x24, area->y1, area->x1, (uint8_t*) color_p, width, height);
    chunkCounter++;

    // Prepare the buffer and add it to collection:
    unsigned char* epd_buffer = new unsigned char[(width * height) / 8];
    memcpy(epd_buffer, color_p, (width * height) / 8);

    ESP_LOGV(TAG_DISPL, "Size of buffer: %i", sizeof(epd_buffer));

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

void epd_set_px_cb_new(_lv_disp_drv_t *disp_drv, uint8_t *buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y,
                       lv_color_t color, lv_opa_t opa)
{
   // Get draw area (absolute coordinates)
   lv_area_t *buf_area = disp_drv->draw_ctx->buf_area;
   int chunk_width = buf_area->x2 - buf_area->x1 + 1;   // Width of the chunk
   int chunk_height = buf_area->y2 - buf_area->y1 + 1; // Height of the chunk

   // Compute number of bytes per column **for this chunk**
   int bytes_per_column = (chunk_height + 7) / 8; // Height in bytes (rounded up)

   // Convert (x, y) to relative positions **within this chunk**
   int relative_x = x;
   int relative_y = y;

   // Ensure pixel is within bounds of the chunk
   if (relative_x < 0 || relative_x >= chunk_width || relative_y < 0 || relative_y >= chunk_height)
       return;

   // Compute buffer position for column-major ordering **within this chunk**
   int column_index = relative_x;
   int row_byte_index = relative_y / 8;
   int bit_position = 7 - (relative_y % 8); // Invert bit position inside byte

   // Determine pixel color (1 = white, 0 = black)
   uint8_t pixel = (lv_color_brightness(color) > 128) ? 1 : 0;

   // **Fix: Compute buffer index relative to the chunk (not full screen!)**
   int buf_index = column_index * bytes_per_column + row_byte_index;

   // Ensure buffer index is within valid range
   if (buf_index < 0 || buf_index >= buf_w * bytes_per_column)
       return;

   // Set or clear the bit in the corresponding byte
   if (pixel)
   {
       buf[buf_index] |= (1 << bit_position);  // Set white pixel (1)
   }
   else
   {
       buf[buf_index] &= ~(1 << bit_position); // Set black pixel (0)
   }
}