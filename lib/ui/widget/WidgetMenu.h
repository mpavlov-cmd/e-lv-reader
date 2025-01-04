#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include "AbstractWidget.h"
#include "model/menu/Menu.h"

class WidgetMenu : public AbstractWidget<Menu>
{

public:
    WidgetMenu() : AbstractWidget() {}
    ~WidgetMenu() {}

private:

    DBox createBox(Menu& widgetData) override
    {
        return widgetData.getBox();
    }

    void initialize(Menu& widgetData) override {

    }

    void beforePrint(Menu& widgetData) override {

    }

    void print(Menu& widgetData) override
    {
        Set<MenuItem> &menuItems = widgetData.getItemsSet();
    }

    void afterPrint(Menu& widgetData) override {
       
    }
};

#endif