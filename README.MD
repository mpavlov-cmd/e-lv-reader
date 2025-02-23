# ESP32 E-Reader Project 

## GxEPD Driver for Reference
- GxEPD2_426_GDEQ0426T82.h
- GxEPD2_426_GDEQ0426T82.cpp

## PSRAM support
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
### Software
- [x] Intent concept: implement action handling and intent swith with LVGL
- [x] Implement StatusManager bakend so actual time, battery etc. are shown
- [x] Implement beautiful clock widget for home page and sleep
- [x] Implement sleep on timer and timer wakeup, so device can update clock and go back to sleep in case of timer wakeup
- [x] Implement config intent, so I can configure clock
- [x] Implement proper sleep aftr inactivity timeout
- [x] Implement proper book page switch, so there are limits
### Hardware
- [x] FIX footprint for crystal
- [x] FIX footprint for joystick (rotate 45 degrees, move to back pannel)
- [x] Move JTAG pin down where josytick was
- [x] Add beter connectors SH 1.0 but horizontal, not vertical
- [x] Add backlight for the screen (Using one of the JTAG pins)
- [x] Try to move LED from IO9 to one of the JTAG pins so either JTAG or led are working
- [x] Verify PIN 23 (IO 15) as LED and PIN 14 (IO 12) as backlight; Verify JTAG still working; On breadboard
- [ ] Configure diff pairs for USB


## WIKI
 - Display size: 800x480  
 - `pio test -vv` will show serial moitor output 
 - `pio test -vv -f "test_textIndex"` filter tests based on a directory
 - [Battery Sense](https://github.com/rlogiacco/BatterySense?tab=readme-ov-file)
 - [SD Card](https://github.com/espressif/arduino-esp32/tree/master/libraries/SD)
 - [Interrupts](https://www.youtube.com/watch?v=CJhWlfkf-5M)
 - [Sleep](https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/)
