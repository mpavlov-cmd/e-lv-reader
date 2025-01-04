#ifndef ABSTRACTWIGDET_H
#define ABSTRACTWIGDET_H

#include <Arduino.h>
#include <esp_log.h>
#include <LogTags.h>
#include <box/DBox.h>

template <typename T>
class AbstractWidget
{
    protected:
        DBox box;

    private:
        bool initialized = false; 

        virtual DBox createBox(T& widgetData) = 0;
        virtual void initialize(T& widgetData) = 0;
        virtual void beforePrint(T& widgetData) = 0;
        virtual void print(T& widgetData) = 0;
        virtual void afterPrint(T& widgetData) = 0;

    public:
        AbstractWidget() : box(DBox()) {};

        virtual ~AbstractWidget() {}

        void upgrade(T& widgetData)
        {

            if (!initialized)
            {
                ESP_LOGD(TAG_WIDGT, "Initializing widget");
                
                // Create box
                box = createBox(widgetData);

                initialize(widgetData);
                initialized = true;
            }

            beforePrint(widgetData);

            // Draw box 
            // display.setPartialWindow(box.x, box.y, box.width, box.height);
            // display.firstPage();
            // do {
                // Diaplay box
                // display.fillRect(box.x, box.y, box.width, box.height, GxEPD_WHITE);
                // display.drawRect(box.x, box.y, box.width, box.height, GxEPD_BLACK);

                // Display widget contents
                print(widgetData);
            // }
            // while (display.nextPage());

            afterPrint(widgetData);
        }
};


#endif
