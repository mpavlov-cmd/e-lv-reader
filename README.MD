# Useful 
lvgl read from sd: https://forum.lvgl.io/t/i-cannot-display-a-jpg-image-from-sd/18179

## Log Levels
```cpp
#define CORE_DEBUG_LEVEL X // Where X is:

ESP_LOGE(TAG, "ESP_LOGE, level 1 = ARDUHAL_LOG_LEVEL_ERROR   = ESP_LOG_ERROR");
ESP_LOGW(TAG, "ESP_LOGW, level 2 = ARDUHAL_LOG_LEVEL_WARN    = ESP_LOG_WARN");    
ESP_LOGI(TAG, "ESP_LOGI, level 3 = ARDUHAL_LOG_LEVEL_INFO    = ESP_LOG_INFO");
ESP_LOGD(TAG, "ESP_LOGD, level 4 = ARDUHAL_LOG_LEVEL_DEBUG   = ESP_LOG_DEBUG");
ESP_LOGV(TAG, "ESP_LOGV, level 5 = ARDUHAL_LOG_LEVEL_VERBOSE = ESP_LOG_VERBOSE"); 
```