[[中文]](./lvgl*wificonfig*cn.md)

# ESP32 LittlevGL Wi-Fi Configuration

This example is no longer maintained. For LCD and LVGL examples, please refer to: [i80*controller](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/lcd/i80*controller)、[rgb*panel](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/lcd/rgb*panel) and [spi*lcd*touch](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/lcd/spi*lcd*touch)

## What You Need

- Hardware:
	* 1 x [ESP32\*LCD\*EB\*V1](https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32/esp32-lcdkit/index.html) HMI development board (for this example, it has to be used with the [ESP32*DevKitC](https://docs.espressif.com/projects/esp-idf/en/stable/hw-reference/modules-and-boards.html#esp32-devkitc-v4) development board)
	* 1 x display (2.8 inches, 240x320 pixels, ILI9341 LCD + XPT2046 Touchscreen)
- Software:
	* [esp-iot-solution](https://github.com/espressif/esp-iot-solution)
	* [LittlevGL GUI](https://lvgl.io/)

See the connection image below:

<div align="center"><img src="../../../docs/*static/hmi*solution/lcd*connect.jpg" width = "700" alt="lcd*connect" align=center /></div>

The pins to be connected:

Name | Pin
-------- | -----
CLK | 22
MOSI | 21
MISO | 27
CS(LCD) | 5
DC | 19
RESET | 18
LED | 23
CS(Touch) | 32
IRQ | 33

## Run the Example

- Open Terminal and navigate to the directory `examples/hmi/lvgl_wificonfig`
- Run `make defconfig`(Make) or `idf.py defconfig`(CMake) to apply the default configuration
- Run `make menuconfig`(Make) or `idf.py menuconfig`(CMake) to set up the flashing-related configuration
- Run `make -j8 flash`(Make) or `idf.py flash`(CMake) to build the example and flash it to the device

## Example Demonstration

<div align="center"><img src="../../../docs/*static/hmi*solution/littlevgl/lvgl*wificonfig0.jpg" width = "700" alt="lvgl*wificonfig" align=center /></div>

<div align="center"><img src="../../../docs/*static/hmi*solution/littlevgl/lvgl*wificonfig1.jpg" width = "700" alt="lvgl*wificonfig" align=center /></div>