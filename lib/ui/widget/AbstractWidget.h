#ifndef ABSTRACTWIGDET_H
#define ABSTRACTWIGDET_H

#include <Arduino.h>
#include <esp_log.h>
#include <LogTags.h>
#include <lvgl.h>
#include <box/DBox.h>

template <typename T>
class AbstractWidget
{
    protected:
         // lvgl group pointer lifeycle managed by intent
        lv_group_t* widgetGroup;

        DBox box;
   
        // lvgl parent object
        lv_obj_t* parent = nullptr;

    private:

        // Widget parent object
        bool initialized = false; 

        virtual DBox createBox(T& widgetData) = 0;
        virtual void initialize(T& widgetData) = 0;
        virtual void beforePrint(T& widgetData) = 0;
        virtual void print(T& widgetData) = 0;
        virtual void afterPrint(T& widgetData) = 0;

    public:
        AbstractWidget(lv_group_t* wGroup) : widgetGroup(wGroup), box(DBox()) {};

        virtual ~AbstractWidget() {
            ESP_LOGD(TAG_WIDGT, "Abstract widget destructor start");
            if (parent != nullptr) {
                lv_obj_del(parent);
            }
            ESP_LOGD(TAG_WIDGT, "Abstract widget destructor end");
        }

        void upgrade(T& widgetData)
        {

            if (!initialized)
            {
                ESP_LOGD(TAG_WIDGT, "Initializing widget");
                
                // Create box
                box = createBox(widgetData);

                parent = lv_obj_create(lv_scr_act()); 
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
