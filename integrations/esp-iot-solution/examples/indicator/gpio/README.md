## LED Indicator WS2812

* Support ON/OFF

### Hardware Required

* LED

### Configure the project

```
idf.py menuconfig
```

* Set `EXAMPLE*GPIO*NUM` to set led gpio.
* Set `EXAMPLE*GPIO*ACTIVE_LEVEL` to set gpio level when led light

### How to USE

If the macro `EXAMPLE*ENABLE*CONSOLE_CONTROL` is enabled, please use the following method for control; otherwise, the indicator lights will flash sequentially in order.

* Help
    ```shell
    help
    ```

* Immediate display mode, without considering priority.
    ```shell
    led -p 0 # Start
    led -p 2 # Start
    led -x 2 # Stop
    ```

* Display mode based on priority.
    ```shell
    led -s 0 # Start 0
    led -s 2 # Start 2
    led -e 2 # Stop 2
    ```

Note:
> Support replacing the LED with an active buzzer to achieve the functionality of a buzzer indicator.