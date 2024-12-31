#include "SwithInputHandler.h"

SwithInputHandler::SwithInputHandler(
    const uint8_t p2, const uint8_t p1, const uint8_t p0
): pin2(p2), pin1(p1), pin0(p0)
{

}

void SwithInputHandler::configure(void (*isrFunction)(), uint8_t debounseThMs, uint16_t holdThMs)
{
    debounseThresholdMills = debounseThMs;
    holdThresholdMills     = holdThMs;

    pinMode(pin0, INPUT);
	pinMode(pin1, INPUT);
	pinMode(pin2, INPUT);

	attachInterrupt(pin0, isrFunction, FALLING);
	attachInterrupt(pin1, isrFunction, FALLING);
	attachInterrupt(pin2, isrFunction, FALLING);    
}

uint8_t SwithInputHandler::handleInput(volatile bool& isrPending, volatile unsigned long& isrStartedAt)
{
    if (!isrPending) {
		return 0;
	}

	// Assure retval is dropped to defauly 
	isrRetVal = 0;
	
	if (millis() - isrLastReturn < debounseThresholdMills) {
		if (!isrInputHeld) {
			// Drop chache to avoid returning value appeared during debounce
			isrInputCache = 0;
		}
		return 0;
	}

	isrInputCurrent = (digitalRead(pin2) << 2) | (digitalRead(pin1) << 1) | digitalRead(pin0);

	if (isrInputCurrent == B00000111) {

		// Only handle next ISR when button was released
		if (isrInputHeld) {
			isrInputHeld = false;
			isrPending   = false;
			return 0;
		}

		isrLastReturn = millis();
		isrPending    = false;

		// Make sure on the next iteration cache is clear
		isrRetVal = isrInputCache;
		isrInputCache = 0;

		return isrRetVal;
	} else {

		if (millis() - isrStartedAt > holdThresholdMills) {
			isrInputHeld  = true;
			isrLastReturn = millis();
			return isrInputCache | B00010000;
		}

		// In case value is 0, set bit 3
		// Chanhe latest non-terminating value
		if (isrInputCurrent == B00000000) {
			isrInputCurrent = isrInputCurrent | B00001000;
		}

		isrInputCache = isrInputCurrent;
	}

	return 0;
}
