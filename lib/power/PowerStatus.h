#ifndef POWERSTATUS_H
#define POWERSTATUS_H

#pragma once

#include <Arduino.h>
#include <Battery.h>
#include <ESP32Time.h>

enum ChargeingStatus { NC, CHARGING, FULL };

struct PowerMetrics
{
    ChargeingStatus chargingStatus;
    bool isConnected;
    uint8_t battLevelPercent;
    uint16_t battVoltageMillivolts;
};


struct PowerStatus
{
    private:
        const float VOLTAGE_DIVIDER_RAITO     = 2.0;
        const uint16_t VOLTAGE_REF_MILLIVOLTS = 3300;
        const uint16_t VOLTAGE_MIN_MILLIVOLTS = 3000;
        const uint16_t VOLTAGE_MAX_MILLIVOLTS = 4200;
        const uint8_t ADC_RESOLUTION          = 12;

        Battery battery;
        uint8_t powerSensePin;
        uint8_t chargeStatusPin;

        uint8_t batteryPercentLatch = 100;
        boolean connectedLatch = false;

        boolean getConnected();
        uint8_t getBatteryLevelPercent();
        ChargeingStatus getChargingStatus();
        uint16_t getBatteryVoltage();

    public:
        PowerStatus(
            uint8_t powerSensePin,
            uint8_t chargeStatusPin, 
            uint8_t batterySensePin
        );

        PowerMetrics measure();
};
 

#endif