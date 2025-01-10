#include "Driver_Input_Keypad.h"

#include <esp_log.h>
#include "LogTags.h"
#include <Arduino.h>

#include "PinDefinitions.h"
#include "SwithInputHandler.h"
#include "ButtonActions.h"

// Function definitions
void IRAM_ATTR isr();
void joystick_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);

// Interrupt variables
volatile bool isrPending = false; 
volatile unsigned long isrStartedAt = 0;

// Input control
static lv_indev_drv_t indev_drv;
static lv_indev_t* indev_keypad;

// Configuration optons
static lv_obj_t* active_object;  
static bool invalidateOnInput = true;

SwithInputHandler inputHandler(BT_INPUT_2, BT_INPUT_1, BT_INPUT_0);

void lv_joystick_indev_init(void)
{
    // ----- INPUT DRIVER INIT START -----
    lv_indev_drv_init(&indev_drv);
    indev_drv.type    = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = joystick_read;
    indev_keypad      = lv_indev_drv_register(&indev_drv);

    inputHandler.configure(isr, 100, 2500);
}

void lv_joystick_invalidate(bool enabled)
{
    invalidateOnInput = enabled;
}

void lv_joystick_active_object(lv_obj_t *object)
{
    active_object = object;
}

lv_indev_t* lv_get_keypad(void)
{
    return indev_keypad;
}

void IRAM_ATTR isr()
{
    if (isrPending) {
		return;
	}

	isrStartedAt = millis();
	isrPending = true;
}

void joystick_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static bool btnPressed = false;
    bool heldBit = false;
    
    uint8_t switchInput = inputHandler.handleInput(isrPending, isrStartedAt);
    if (switchInput) {

        ESP_LOGD(TAG_INPUT, "Button Pressed");

        // TODO: Come up with better optimization
        if (invalidateOnInput) {
            lv_obj_t* to_invalidate = active_object == nullptr ? lv_scr_act() : active_object;
            lv_obj_invalidate(to_invalidate);
        }

        // Mark btn as pressed 
        data->state = LV_INDEV_STATE_PRESSED;
        btnPressed  = true;

        const uint8_t directionInput = controlDirection(switchInput, heldBit);

        if (directionInput == BUTTON_ACTION_LEFT) {
            data->key = LV_KEY_PREV;
        }

        if (directionInput == BUTTON_ACTION_RIGHT) {
            data->key = LV_KEY_NEXT;
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

        // Send additional event to make sure key is registred
        uint32_t currentKey = data->key;
        if (currentKey == LV_KEY_PREV || currentKey == LV_KEY_NEXT) {
            uint32_t keyToSend = currentKey == LV_KEY_PREV ? LV_KEY_LEFT : LV_KEY_RIGHT;
            lv_event_send(lv_group_get_focused(lv_group_get_default()), LV_EVENT_KEY, &keyToSend);
        }

    } else {
        // TODO: Figure out wy released is triggered while held
        if (btnPressed) {
            ESP_LOGD(TAG_INPUT, "Button Released");

            btnPressed  = false; 
            data->state = LV_INDEV_STATE_RELEASED;
        }
    }
}