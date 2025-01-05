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

    // lvgl parent object
    lv_obj_t* parent;

    // Common event handler
    static void eventHandler(lv_event_t *event)
    {
        lv_obj_t *target = lv_event_get_target(event);
        lv_event_code_t code = lv_event_get_code(event);

        // ESP_LOGD(TAG_WIDGT, "Event received for target: %p, code: %d", target, code);
        // Retrieve the widget instance and its event queue (passed as user data)
        AbstractWidget* widget = static_cast<AbstractWidget*>(lv_event_get_user_data(event));
        if (widget && widget->eventQueue)
        {
            ActionArgument argumnet = {target, code};
            if (xQueueSend(widget->eventQueue, &argumnet, pdMS_TO_TICKS(10)) != pdPASS)
            {
                ESP_LOGW(TAG_WIDGT, "Failed to send event to queue");
            }
        }
    }

    // Method to attach the event handler to an object
    void attachEventHandler(lv_obj_t *obj)
    {
        lv_obj_add_event_cb(obj, eventHandler, LV_EVENT_ALL, this); // Pass contextData if needed
    }

private:
    // Widget parent object
    bool initialized = false;

    virtual DBox createBox(T &widgetData) = 0;
    virtual void initialize(T &widgetData) = 0;
    virtual void beforePrint(T &widgetData) = 0;
    virtual void print(T &widgetData) = 0;
    virtual void afterPrint(T &widgetData) = 0;

public:
    AbstractWidget(lv_group_t *wGroup, lv_obj_t* mParent, QueueHandle_t& mEventQueue) : 
        widgetGroup(wGroup), parent(mParent), eventQueue(mEventQueue), box(DBox()) {};

    virtual ~AbstractWidget()
    {
        ESP_LOGD(TAG_WIDGT, "Abstract widget destructor start");
        if (parent != nullptr)
        {
            lv_obj_del(parent);
        }
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

            initialize(widgetData);
            initialized = true;
        }

        beforePrint(widgetData);
        print(widgetData);
        afterPrint(widgetData);
    }
};

#endif
