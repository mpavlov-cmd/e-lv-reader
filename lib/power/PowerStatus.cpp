#include "PowerStatus.h"

PowerStatus::PowerStatus(
    uint8_t powerSensePin,
    uint8_t chargeStatusPin,
    uint8_t batterySensePin
) : 
        battery(VOLTAGE_MIN_MILLIVOLTS, VOLTAGE_MAX_MILLIVOLTS, batterySensePin, ADC_RESOLUTION),
        powerSensePin(powerSensePin), 
        chargeStatusPin(chargeStatusPin)
{
    pinMode(powerSensePin, INPUT);
    pinMode(chargeStatusPin, INPUT);

    // Set 12 bit ADC resolution
    analogReadResolution(ADC_RESOLUTION);
    battery.begin(VOLTAGE_REF_MILLIVOLTS, VOLTAGE_DIVIDER_RAITO, &asigmoidal);
}

boolean PowerStatus::getConnected()
{
    return digitalRead(powerSensePin) == HIGH;
}

uint8_t PowerStatus::getBatteryLevelPercent()
{
    // If power not connected, battery percent should only decrease
    boolean currentlyConnected = getConnected();

    if (!currentlyConnected) {
         uint8_t levelReading   = battery.level();

        if (batteryPercentLatch > levelReading) {
            batteryPercentLatch = levelReading;
        } 
    }

    connectedLatch = currentlyConnected;
    return batteryPercentLatch;
}

uint16_t PowerStatus::getBatteryVoltage()
{
    return battery.voltage();
}

// TODO: Implement logic to indicate battery not connected
ChargeingStatus PowerStatus::getChargingStatus()
{
    return digitalRead(chargeStatusPin) == LOW ? CHARGING : FULL;
}


PowerMetrics PowerStatus::measure()
{
    ChargeingStatus chargeingStatus = getChargingStatus();
    bool isConnected = getConnected();
    uint8_t battLevelPercent = getBatteryLevelPercent();
    uint16_t voltage = getBatteryVoltage();

    return {chargeingStatus, isConnected, battLevelPercent, voltage};
}