| Supported Targets | ESP32-S3 |
| ----------------- | -------- |

| Supported LCD Controllers | ST77903 |
| ------------------------- | ------- |

# RGB LCD 8BIT Example

[esp*lcd](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/lcd.html) provides several panel drivers out-of box, e.g. ST7789, SSD1306, NT35510. However, there're a lot of other panels on the market, it's beyond `esp*lcd` component's responsibility to include them all.

`esp*lcd` allows user to add their own panel drivers in the project scope (i.e. panel driver can live outside of esp-idf), so that the upper layer code like LVGL porting code can be reused without any modifications, as long as user-implemented panel driver follows the interface defined in the `esp*lcd` component.

This example demonstrates how to drive an RGB interface LCD screen with an 8-bit RGB requirement in an esp-idf project. The example will use the LVGL library to draw a stylish music player.

The LVGL-related parameter configurations, such as LVGL's registered resolution, LVGL task-related parameters, and tearing prevention methods, can be configured in lvgl*port*v9.h.

This example uses the [esp*timer](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp*timer.html) to generate the ticks needed by LVGL and uses a dedicated task to run the `lv*timer*handler()`. Since the LVGL APIs are not thread-safe, this example uses a mutex which be invoked before the call of `lv*timer*handler()` and released after it. The same mutex needs to be used in other tasks and threads around every LVGL (lv_...) related function call and code. For more porting guides, please refer to [LVGL porting doc](https://docs.lvgl.io/master/porting/index.html).

## How to use the example

## ESP-IDF Required

### Hardware Required

* An ESP32-S3R8 development board
* A ST77903 LCD panel, with RGB interface
* An USB cable for power supply and programming

### Hardware Connection

The connection between ESP Board and the LCD is as follows:

```
       ESP Board                           RGB  Panel
+-----------------------+              +-------------------+
|                   GND +--------------+GND                |
|                       |              |                   |
|                   3V3 +--------------+VCC                |
|                       |              |                   |
|                   PCLK+--------------+PCLK               |
|                       |              |                   |
|              DATA[7:0]+--------------+DATA[7:0]          |
|                       |              |                   |
|                  HSYNC+--------------+HSYNC              |
|                       |              |                   |
|                  VSYNC+--------------+VSYNC              |
|                       |              |                   |
|                     DE+--------------+DE                 |
|                       |              |                   |
|                     CS+--------------+CS                 |
|                       |              |                   |
|                    SCK+--------------+SCK                |
|                       |              |                   |
|                    SDA+--------------+SDA                |
|                       |              |                   |
|               BK_LIGHT+--------------+BLK                |
+-----------------------+              |                   |
                               3V3-----+DISP_EN            |
                                       |                   |
                                       +-------------------+
```

* The LCD parameters and GPIO number used by this example can be changed in [example*rgb*lcd*8bit.c](main/example*rgb*lcd*8bit.c). Especially, please pay attention to the **vendor specific initialization**, it can be different between manufacturers and should consult the LCD supplier for initialization sequence code.
* The LVGL parameters can be changed not only through `menuconfig` but also directly in `lvgl_conf.h`

### Configure the Project

Run `idf.py menuconfig` and navigate to `Example Configuration` menu.

### Build and Flash

Run `idf.py set-target esp32s3` to select the target chip.

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project. A fancy animation will show up on the LCD as expected.

The first time you run `idf.py` for the example will cost extra time as the build system needs to address the component dependencies and downloads the missing components from registry into `managed_components` folder.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-iot-solution/issues) on GitHub. We will get back to you soon.
