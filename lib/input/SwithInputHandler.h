#ifndef SWITCHINPUTHANDLER_H
#define SWITCHINPUTHANDLER_H

#include <Arduino.h>

struct SwithInputHandler
{
    private:
        const uint8_t pin2, pin1, pin0;

        bool isrInputHeld = false;
        unsigned long isrLastReturn = 0;
        uint8_t isrInputCache = 0, isrInputCurrent = 0, isrRetVal = 0, debounseThresholdMills = 100;
        uint16_t holdThresholdMills = 2500;

    public: 
        /**
         * Construct handler. Add 3 input pints, starting MSB first
         * 
         * @param p2 - pin 2
         * @param p1 - pin 1
         * @param p0 - pin 0
        */
        SwithInputHandler(const uint8_t p2, const uint8_t p1, const uint8_t p0);

        /** 
         * Set up ISR functaion, attach interrupts and set debounce and hold threshold
         * ISR will be triggered during falling edge as by default (non pressed) all 3 inputs are high 
         * 
         * @param isrFunction self explenatory
         * @param debounseThMs minimal time interval between 2 button press in milliseconds
         * @param holdThMs minimal time interval after wich handleInputFuncyion will detect hold
        */
        void configure(void (*isrFunction)(), uint8_t debounseThMs, uint16_t holdThMs);


        /**
         * Handlers user input and will return:
         * 
         * @param isrPending wether function should handle isr
         * @param isrStartedAt time in milliseconds since interrupt detected to handle long press
         * 
         * @returns value depending on pressed key
         * 0b00000000 - no button pressed
         * 
         * 0b00000011 - right key press
         * 0b00000100 - left key press, 
         * 0b00000001 - up key press
         * 0b00000010 - down key press, 
         * 0b00001000 - mid button press
         * 
         * when button held returns the same valuse as above but with bit 5 set high
         *  
         */
        uint8_t handleInput(volatile bool& isrPending, volatile unsigned long& isrStartedAt);
};


#endif