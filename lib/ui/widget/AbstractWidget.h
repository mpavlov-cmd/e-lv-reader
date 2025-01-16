#ifndef ABSTRACTWIGDET_H
#define ABSTRACTWIGDET_H

#include <Arduino.h>
#include <esp_log.h>
#include <LogTags.h>
#include <lvgl.h>
#include <box/DBox.h>
#include <ActionArgument.h>

template <typename T>
class AbstractWidget
{
protected:
    // lvgl group pointer lifeycle managed by intent
    lv_group_t* widgetGroup;

    QueueHandle_t& eventQueue;

    DBox box;

    // lvgl parent object and style
    lv_obj_t* parent;
    lv_style_t style;

    // Common event handler
    static void eventHandler(lv_event_t *event)
    {
        lv_obj_t *target = lv_event_get_target(event);
        lv_event_code_t code = lv_event_get_code(event);

        ESP_LOGD(TAG_WIDGT, "Event received for target: %p, code: %d", target, code);
        // Retrieve the widget instance and its event queue (passed as user data)
        AbstractWidget* widget = static_cast<AbstractWidget*>(lv_event_get_user_data(event));
        if (widget && widget->eventQueue)
        {
            ActionArgument argumnet = {target, code};
            if (xQueueSend(widget->eventQueue, &argumnet, pdMS_TO_TICKS(10)) != pdPASS)
            {
                ESP_LOGE(TAG_WIDGT, "Failed to send event to ui queue");
            }
        }
    }

    // Method to attach the event handler to an object
    void attachEventHandler(lv_obj_t *obj)
    {
        // Add more events here if neded
        lv_obj_add_event_cb(obj, eventHandler, LV_EVENT_CLICKED, this);
        lv_obj_add_event_cb(obj, eventHandler, LV_EVENT_FOCUSED, this);
        lv_obj_add_event_cb(obj, eventHandler, LV_EVENT_KEY, this);
    }

private:
    // Widget parent object
    bool initialized = false;

    virtual DBox createBox(T &widgetData) = 0;
    virtual void initialize(T &widgetData) = 0;
    virtual void print(T &widgetData) = 0;

public:
    // Primary 
    AbstractWidget(lv_group_t *wGroup, lv_obj_t* mParent, QueueHandle_t& mEventQueue) : 
        widgetGroup(wGroup), parent(mParent), eventQueue(mEventQueue), box(DBox::EMPTY) {};

    AbstractWidget(lv_group_t *wGroup, QueueHandle_t& mEventQueue) : 
        AbstractWidget(wGroup, nullptr, mEventQueue) {};

    virtual ~AbstractWidget()
    {
        ESP_LOGD(TAG_WIDGT, "Abstract widget destructor start");
        if (lv_obj_is_valid(parent))
        {
            lv_obj_del(parent);
        }
        lv_style_reset(&style);
        ESP_LOGD(TAG_WIDGT, "Abstract widget destructor end");
    }

    void upgrade(T &widgetData)
    {
        if (!initialized)
        {
            ESP_LOGD(TAG_WIDGT, "Initializing widget");

            // Create box
            box = createBox(widgetData);

            if (parent == nullptr) {
                parent = lv_obj_create(lv_scr_act());
            }

            lv_obj_set_size(parent, box.width, box.height);
            lv_obj_set_pos(parent, box.x, box.y);

            lv_style_init(&style);
            lv_style_set_border_width(&style, box.border);
            lv_style_set_border_color(&style, lv_color_black());
            lv_style_set_border_opa(&style, LV_OPA_COVER);
            lv_style_set_pad_all(&style, box.padding);    

            lv_obj_add_style(parent, &style, LV_PART_MAIN);

            initialize(widgetData);
            initialized = true;
        }

        print(widgetData);
    }
};

#endif
