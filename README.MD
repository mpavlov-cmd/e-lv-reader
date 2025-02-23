# ESP32 E-Reader Project 
Opensource ESP-32 EPaper Display Device
![TitleImage](https://raw.githubusercontent.com/mpavlov-cmd/e-lv-reader/refs/heads/main/img/Title.jpg)
![PCB](https://raw.githubusercontent.com/mpavlov-cmd/e-lv-reader/refs/heads/main/img/PCB.jpg)

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
- [ ] Adjust MODIFY README.MD further
### Hardware
- [ ] Implement diff pairs for USB
- [ ] Move Battery Connector so it does not create an issue with housing
- [ ] Integrate DESPI-C02 into PCB
- [ ] Position ESP-32 chip so the antenna is placed properly


## WIKI
 - Display size: 800x480  
 - `pio test -vv` will show serial moitor output 
 - `pio test -vv -f "test_textIndex"` filter tests based on a directory
 - [Battery Sense](https://github.com/rlogiacco/BatterySense?tab=readme-ov-file)
 - [SD Card](https://github.com/espressif/arduino-esp32/tree/master/libraries/SD)
 - [Interrupts](https://www.youtube.com/watch?v=CJhWlfkf-5M)
 - [Sleep](https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/)
