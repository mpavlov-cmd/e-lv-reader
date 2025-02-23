# ESP32 E-Reader Project 
Opensource ESP-32 EPaper Display Device

## Discalimer
Project comes with ABSOLUTELY NO WARRANTY. Author is not responsible for any damage made to individuals or their property while attempting to build or/and use the product.

## Features
### Hardware
> [!NOTE]
> Schematic, gerber and 3mf files for building PCB and housing are availble under `hardware` folder of the repository.

- Popular and cheap ESP32 platform.
- Big 800x480 Good Display GDEQ0426T82 E-Paper SPI Diaplay.
- USB-C power, charing and firmware upload.
- Micro SD Card support (SPI Interface).
- Low power consumption: ~2mA in sleep mode.
- Optinal JTAG connector for debug purposes.
- External clock crystal for accurate time. 
- Ability to control external hardware e.g., backlight.
- Power and battery level dtection.
- All components available on aliexpress almost for free.
- 30th/15th vias for lowering PCB manufaturing cost.

> [!WARNING]
> Currently PCB is using external DESPI-C02 driver board for interfacing with EPD.
> This will potentially be changed in the further imppementations as external driver board does not allow to make thin housing design.

### Part List
Coming soon

### Software
- PlatformIO, Arduino Framework.
- LVGL driver for GDEQ0426T82 display with partial refresh 2 buffers and partial rendering support.
- MVC framework featuring abstract Intent and Widget concepts.

## Pictures
![Housing](https://raw.githubusercontent.com/mpavlov-cmd/e-lv-reader/refs/heads/main/img/Title.jpg)
![PCB](https://raw.githubusercontent.com/mpavlov-cmd/e-lv-reader/refs/heads/main/img/PCB.jpg)

## Software Used
- [Labcenter Proteus](https://www.labcenter.com/)
- [FreeCAD](https://www.freecad.org/)
- [Platform IO](https://platformio.org/)
- [Visual Studio Code](https://code.visualstudio.com/download)

## Special Thanks
- [Andre' Lamothe](https://www.udemy.com/course/crash-course-electronics-and-pcb-design/?couponCode=JUST4U02223) for sharing his passion for electronics with so many people
- [Ben Eater](https://eater.net/)
- [@MangoJellySolutions](https://www.youtube.com/@MangoJellySolutions) for a great FreeCAD course

# Project Notes

## No PSRAM Support
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
Converting image with GIMP:

- Use Image -> Mode -> Indexed with 256 colours and dithering if needed.
- Export as a filename.bmp, the Advanced Options are greyed out.
- Depending on purpose you might need to toggle color space info on/off.

## TODO List
### Software
- [ ] Further adjust README.MD further
### Hardware
- [ ] Implement diff pairs for USB
- [ ] Move battery, backlight Connectos, so they do not interfere with housing 
- [ ] Integrate DESPI-C02 into PCB
- [ ] Position ESP-32 chip so the antenna is placed outside of the board

## WIKI
 - Display: 800x480  
 - `pio test -vv` will show serial moitor output 
 - `pio test -vv -f "test_textIndex"` filter tests based on a directory
 - [Battery Sense](https://github.com/rlogiacco/BatterySense?tab=readme-ov-file)
 - [SD Card](https://github.com/espressif/arduino-esp32/tree/master/libraries/SD)
 - [Interrupts](https://www.youtube.com/watch?v=CJhWlfkf-5M)
 - [Sleep](https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/)
