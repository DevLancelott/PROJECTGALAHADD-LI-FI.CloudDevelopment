| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Basic Console Example (`esp*console*repl`)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example illustrates the usage of the REPL (Read-Eval-Print Loop) APIs of the [Console Component](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/console.html#console) to create an interactive shell on the ESP chip. The interactive shell running on the ESP chip can then be controlled/interacted with over a serial interface. This example supports UART and USB interfaces.

The interactive shell implemented in this example contains a wide variety of commands, and can act as a basis for applications that require a command-line interface (CLI).

Compared to the [advanced console example](../advanced), this example requires less code to initialize and run the console. `esp*console*repl` API handles most of the details. If you'd like to customize the way console works (for example, process console commands in an existing task), please check the advanced console example.

## How to use example

This example can be used on boards with UART and USB interfaces. The sections below explain how to set up the board and configure the example.

### Using with UART

When UART interface is used, this example should run on any commonly available Espressif development board. UART interface is enabled by default (`CONFIG*ESP*CONSOLE*UART*DEFAULT` option in menuconfig). No extra configuration is required.

### Using with USB*SERIAL*JTAG

*NOTE: We recommend to disable the secondary console output on chips with USB*SERIAL*JTAG since the secondary serial is output-only and would not be very useful when using a console application. This is why the secondary console output is deactivated per default (CONFIG*ESP*CONSOLE*SECONDARY*NONE=y)*

On chips with USB*SERIAL*JTAG peripheral, console example can be used over the USB serial port.

* First, connect the USB cable to the USB*SERIAL*JTAG interface.
* Second, run `idf.py menuconfig` and enable `CONFIG*ESP*CONSOLE*USB*SERIAL_JTAG` option.

For more details about connecting and configuring USB*SERIAL*JTAG (including pin numbers), see the IDF Programming Guide:
* [ESP32-C3 USB*SERIAL*JTAG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c3/api-guides/usb-serial-jtag-console.html)
* [ESP32-S3 USB*SERIAL*JTAG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/usb-serial-jtag-console.html)

### Using with USB CDC (USB_OTG peripheral)

USB_OTG peripheral can also provide a USB serial port which works with this example.

* First, connect the USB cable to the USB_OTG peripheral interface.
* Second, run `idf.py menuconfig` and enable `CONFIG*ESP*CONSOLE*USB*CDC` option.

For more details about connecting and configuring USB_OTG (including pin numbers), see the IDF Programming Guide:
* [ESP32-S2 USB_OTG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s2/api-guides/usb-otg-console.html)
* [ESP32-S3 USB_OTG](https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/api-guides/usb-otg-console.html)

### Other configuration options

This example has an option to store the command history in Flash. This option is enabled by default.

To disable this, run `idf.py menuconfig` and disable `CONFIG*CONSOLE*STORE_HISTORY` option.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Enter the `help` command get a full list of all available commands. The following is a sample session of the Console Example where a variety of commands provided by the Console Example are used.

On ESP32, GPIO15 may be connected to GND to remove the boot log output.

```
This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
[esp32]> help
help
  Print the list of registered commands

free
  Get the total size of heap memory available

restart
  Restart the program

deep_sleep  [-t <t>] [--io=<n>] [--io_level=<0|1>]
  Enter deep sleep mode. Two wakeup modes are supported: timer and GPIO. If no
  wakeup option is specified, will sleep indefinitely.
  -t, --time=<t>  Wake up time, ms
      --io=<n>  If specified, wakeup using GPIO with given number
  --io_level=<0|1>  GPIO level to trigger wakeup

join  [--timeout=<t>] <ssid> [<pass>]
  Join WiFi AP as a station
  --timeout=<t>  Connection timeout, ms
        <ssid>  SSID of AP
        <pass>  PSK of AP

[esp32]> free
257200
[esp32]> deep_sleep -t 1000
I (146929) deep_sleep: Enabling timer wakeup, timeout=1000000us
I (619) heap_init: Initializing. RAM available for dynamic allocation:
I (620) heap_init: At 3FFAE2A0 len 00001D60 (7 KiB): DRAM
I (626) heap_init: At 3FFB7EA0 len 00028160 (160 KiB): DRAM
I (645) heap_init: At 3FFE0440 len 00003BC0 (14 KiB): D/IRAM
I (664) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (684) heap_init: At 40093EA8 len 0000C158 (48 KiB): IRAM

This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
[esp32]> join --timeout 10000 test_ap test_password
I (182639) connect: Connecting to 'test_ap'
I (184619) connect: Connected
[esp32]> free
212328
[esp32]> restart
I (205639) restart: Restarting
I (616) heap_init: Initializing. RAM available for dynamic allocation:
I (617) heap_init: At 3FFAE2A0 len 00001D60 (7 KiB): DRAM
I (623) heap_init: At 3FFB7EA0 len 00028160 (160 KiB): DRAM
I (642) heap_init: At 3FFE0440 len 00003BC0 (14 KiB): D/IRAM
I (661) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (681) heap_init: At 40093EA8 len 0000C158 (48 KiB): IRAM

This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
[esp32]>

```

## Troubleshooting

### Line Endings

The line endings in the Console Example are configured to match particular serial monitors. Therefore, if the following log output appears, consider using a different serial monitor (e.g. Putty for Windows) or modify the example's [UART configuration](#Configuring-UART-and-VFS).

```
This is an example of ESP-IDF console component.
Type 'help' to get the list of commands.
Use UP/DOWN arrows to navigate through command history.
Press TAB when typing command name to auto-complete.
Your terminal application does not support escape sequences.
Line editing and history features are disabled.
On Windows, try using Windows Terminal or Putty instead.
esp32>
```

### Escape Sequences on Windows 10

When using the default command line or PowerShell on Windows 10, you may see a message indicating that the console does not support escape sequences, as shown in the above output. To avoid such issues, it is recommended to run the serial monitor under [Windows Terminal](https://en.wikipedia.org/wiki/Windows_Terminal), which supports all required escape sequences for the app, unlike the default terminal. The main escape sequence of concern is the Device Status Report (`0x1b[5n`), which is used to check terminal capabilities. Any response to this sequence indicates support. This should not be an issue on Windows 11, where Windows Terminal is the default.
