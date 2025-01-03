# Useful 
- lvgl read from sd: https://forum.lvgl.io/t/i-cannot-display-a-jpg-image-from-sd/18179
- sd driver sample: https://github.com/100askTeam/esp-arduino-learn/blob/master/examples/08_integrated/01_lcd_sd_card_fc_joypad_fs_lv_lib_100ask/01_lcd_sd_card_fc_joypad_fs_lv_lib_100ask.ino

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
