#ifndef SET_H
#define SET_H

#include "SetBase.h" // Include the abstract base class

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
        #if defined(SERIAL_DEBUG_ENABLED)
        Serial.println("Entering set Distructor");
        #endif
        
        // Delete all dynamically allocated items
        clear();

        #if defined(SERIAL_DEBUG_ENABLED)
        Serial.println("Before array delete");
        #endif
        
        if (itemsArray != nullptr)
        {
            delete[] itemsArray;
            itemsArray = nullptr;
        }

        #if defined(SERIAL_DEBUG_ENABLED)
        Serial.println("Exiting set distructor");
        #endif
    }

    // Copy Constructor (Deep Copy)
    Set(const Set &other) : index(0), capacity(other.capacity)
    {
        #if defined(SERIAL_DEBUG_ENABLED)
        Serial.println("Entering copy constructor");
        #endif

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

        #if defined(SERIAL_DEBUG_ENABLED)
        Serial.printf("Assignment done Old size: %i, new size: %i\n", other.size(), size());
        #endif
        
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
        #if defined(SERIAL_DEBUG_ENABLED)
        Serial.println("Inside of clear method");
        #endif
        while (index > 0)
        {
            #if defined(SERIAL_DEBUG_ENABLED)
            Serial.printf("Deleting index: %i\n", index);
            #endif

            delete itemsArray[--index]; // Delete each item
            itemsArray[index] = nullptr;
        }

        #if defined(SERIAL_DEBUG_ENABLED)
        Serial.println("Clear completed");
        #endif
    }
};

#endif