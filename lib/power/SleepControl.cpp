#include "SleepControl.h"
#include "esp_log.h"
#include "LogTags.h"

SleepControl::SleepControl(SleepControlConf config) : config(config)
{
}

void SleepControl::sleepNow()
{
    if (extWakeUpConfResult != ESP_OK) {
        ESP_LOGE(TAG_POWER, "Unable to sleep, configuration failure: %i", extWakeUpConfResult);
        return;
    }

    ESP_LOGI(TAG_POWER, "Entering deep sleep. Zzzz...");
    esp_deep_sleep_start();
}

bool SleepControl::setWarkupTimer(uint16_t wakeupTimerSecons)
{
    esp_err_t result = esp_sleep_enable_timer_wakeup(wakeupTimerSecons * US_TO_S_FACTOR);
    ESP_LOGD(TAG_POWER, "Set wake up timer, reult: %i", result);

    return result == ESP_OK;
}

esp_sleep_wakeup_cause_t SleepControl::getWarkeupCause()
{
    return esp_sleep_wakeup_cause_t();
}

void SleepControl::configureExt1WakeUp()
{
    extWakeUpConfResult = esp_sleep_enable_ext1_wakeup(config.gpioMask, config.mode);
    ESP_LOGI(TAG_POWER, "Ext1 wakeup configured: %i", extWakeUpConfResult);
}
