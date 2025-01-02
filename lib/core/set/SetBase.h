#ifndef SETBASE_H
#define SETBASE_H

#include <Arduino.h>

template <typename T>
class SetBase
{
public:
    virtual ~SetBase() {}

    virtual bool addItem(T *item) = 0;
    virtual bool removeItem() = 0;
    virtual T *getItem(uint16_t idx) const = 0;
    virtual uint16_t size() const = 0;
    virtual void clear() = 0;
};

#endif