| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Custom Flash Driver Example

This example shows how to override the default chip driver list provided by IDF. Please make sure the SPI*FLASH*OVERRIDE*CHIP*DRIVER*LIST config option is enabled when you build this project (though it should already be enabled by default via `sdkconfig.defaults`). See [the programming guide](https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/storage/spi*flash*override*driver.html) for more details regarding this feature.

**CAUTION: This is only an example showing how to extend your own flash chip driver. Espressif doesn't guarantee the chip driver in the example is reliable for mass production, nor the reliability of the flash models that appear in this example. Please refer to the specification of your flash chip, or contact the flash vendor if you have any problems with the flash chip.**

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## How to use example

Follow detailed instructions provided specifically for this example.

Select the instructions depending on Espressif chip installed on your development board:

- [ESP32 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/stable/get-started/index.html)
- [ESP32-S2 Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)


## Example folder contents

The project **custom*flash*driver** contains one source file in C language [main.c](main/main.c). The file is located in folder [main](main).

The component **custom*chip*driver** placed under **components** folder, provides

ESP-IDF projects are built using CMake. The project build configuration is contained in `CMakeLists.txt` files that provide set of directives and instructions describing the project's source files and targets (executable, library, or both).

Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── sdkconfig.defaults         Default options to add into sdkconfig file (mainly to enable the SPI_FLASH_OVERRIDE_CHIP_DRIVER_LIST option)
├── example_test.py            Python script used for automated example testing
├── main
│   ├── CMakeLists.txt
│   └── main.c
├── components/custom_chip_driver
│   ├── CMakeLists.txt
│   ├── linker.lf              Linker script to put the customized chip driver into internal RAM
│   ├── chip_drivers.c
│   ├── idf_component.yml      Component manager for flash driver component
├── bootloader_components/bootloader_flash
│   ├── bootloader_flash_qio_custom.c
|   ├── bootloader_flash_unlock_custom.c
│   ├── CMakeLists.txt
│   ├── idf_component.yml      Component manager for flash driver component
└── README.md                  This is the file you are currently reading
```

For more information on structure and contents of ESP-IDF projects, please refer to Section [Build System](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html) of the ESP-IDF Programming Guide.

## How to implement your driver

Please read [instructions](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi*flash/spi*flash*override*driver.html)

## Example output

The example output will be valid when you switch on `CONFIG*BOOTLOADER*LOG*LEVEL*DEBUG`， and see output in bootloader stage

```
I (30) boot: ESP-IDF v5.4-dev-2463-g0d6cf47e5e-dirty 2nd stage bootloader
...
I (45) boot: chip revision: v0.3
D (49) boot.esp32c3: Using overridden bootloader_flash_unlock
...
D (69) qio_mode: Using overridden bootloader_flash_qio
```

## Troubleshooting

* Program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

## Technical support and feedback

Please use the following feedback channels:

* For technical queries, go to the [esp32.com](https://esp32.com/) forum
* For a feature request or bug report, create a [GitHub issue](https://github.com/espressif/esp-idf/issues)

We will get back to you as soon as possible.
