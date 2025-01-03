#ifndef SET_H
#define SET_H

#include "SetBase.h" // Include the abstract base class
#include "esp_log.h"
#include "LogTags.h"

template <typename T>
class Set : public SetBase<T>
{
private:
    uint16_t capacity;
    uint16_t index;
    T **itemsArray;

public:

    /** 
    * Creates set of a given max capacity
    * 
    * @param itemsCount max items in the set 
    */
    Set(uint16_t itemsCount) : index(0), capacity(itemsCount)
    {
        // Initialize array with null pointers
        itemsArray = new T *[capacity](); 
    }

    // Destructor
    ~Set()
    {
        ESP_LOGD(TAG_SET, "Entering set distructor");
        // Delete all dynamically allocated items
        clear();

        ESP_LOGD(TAG_SET, "Before array delete");
        if (itemsArray != nullptr)
        {
            delete[] itemsArray;
            itemsArray = nullptr;
        }

        ESP_LOGD(TAG_SET, "Exiting set distructor");
    }

    // Copy Constructor (Deep Copy)
    Set(const Set &other) : index(0), capacity(other.capacity)
    {
        ESP_LOGD(TAG_SET, "Entering copy constructor");

        itemsArray = new T *[capacity]();
        // Deep copy each item
        for (uint16_t i = 0; i < other.size(); i++)
        {
            addItem(new T(*other.getItem(i)));
        }
    }

    // Assignment Operator (Deep Copy)
    Set &operator=(const Set &other)
    {
        if (this == &other) {
            return *this;
        }
        
        // Free existing data
        clear();
        delete[] itemsArray;

        itemsArray = new T *[other.capacity]();
        // Deep copy each item
        for (uint16_t i = 0; i < other.size(); i++)
        {
            addItem(new T(*other.getItem(i)));
        }

        ESP_LOGD(TAG_SET, "Assignment done Old size: %i, new size: %i\n", other.size(), size());
        
        return *this;
    }

    /**
     * Add an item to the set
     *
     * @param item pointer to the new item
     * @return true if added, false if the set is at full capacity
     */
    bool addItem(T *item) override
    {
        if (index >= capacity)
        {
            return false; // Check if capacity is full
        }

        itemsArray[index] = item; // Add the item and increment index
        index++;

        return true;
    }

    /**
     * Remove the last item from the set
     * @return true if removed, false if the set is empty
     */
    bool removeItem() override
    {
        if (index == 0)
        {
            // Set is empty
            return false;
        }

        index--;

        // Delete the last item and decrement index
        delete itemsArray[index]; 
        itemsArray[index] = nullptr;

        return true;
    }

    /**
     * Get an item at a specified index
     *
     * @param idx Index of the target item
     * @return pointer to the item, or nullptr if the index is invalid
     */
    T *getItem(uint16_t idx) const override
    {
        if (idx >= index)
        {
            return nullptr; // Check bounds and return nullptr if invalid
        }

        return itemsArray[idx];
    }

    /**
     * Get the current size of the set
     * @return Number of elements in the set
     */
    uint16_t size() const override
    {
        return index;
    }

    /**
     * Clear all items in the set
     */
    void clear() override
    {
        ESP_LOGD(TAG_SET, "Inside of clear method");
        while (index > 0)
        {
            ESP_LOGD(TAG_SET, "Deleting index: %i", index);

            delete itemsArray[--index]; // Delete each item
            itemsArray[index] = nullptr;
        }

        ESP_LOGD(TAG_SET, "Clear completed");
    }
};

#endif