# ESP32 E-Reader Project 

## GxEPD Driver
- GxEPD2_426_GDEQ0426T82.h
- GxEPD2_426_GDEQ0426T82.cpp

## PS Ram support (No)
- Chip Used: ESP32-D0WD-V3
- Chip with PSRAM: ESP32-D0WDR2-V3 (R - has ram, 2 - size in megs)

## Log Levels
```ini
build_flags =
    ; ESP Log level
    -D CORE_DEBUG_LEVEL=4
```

## Supported 8 Bit BMPs
Set `lv_conf.h` parameter, in my case in `platfromio.ini`:
```ini
build_flags =
    -D LV_COLOR_DEPTH=8
```
To convert image use:

- Use Image -> Mode -> Indexed with 256 colours and dithering if needed.
- Export as a filename.bmp, the Advanced Options are greyed out.
- Depending on purpose you might need to toggle color space info on/off.

## TODO List
- [ ] Intent concept: implement action handling and intent swith with LVGL