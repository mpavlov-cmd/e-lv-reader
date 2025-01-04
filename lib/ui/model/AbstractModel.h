#ifndef ABSTARCTMODEL_H
#define ABSTARCTMODEL_H

#include <box/DBox.h>

struct AbstractModel
{
    private:
        DBox box;

    public:
        AbstractModel(DBox& box) : box(box) {};

        DBox getBox() {
            return box;
        };
};


#endif