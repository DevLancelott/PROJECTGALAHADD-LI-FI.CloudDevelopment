[![Component Registry](https://components.espressif.com/components/espressif/iot*usbh*ecm/badge.svg)](https://components.espressif.com/components/espressif/iot*usbh*ecm)

## IOT USBH ECM Component

`iot*usbh*ecm` is a USB `ECM` device driver for ESP32-S2/ESP32-S3/ESP32-P4. It acts as a USB host to connect ECM devices.

Features:

1. Support hot-plug of ECM devices
2. Support auto-detection and connection of ECM devices
3. Support auto-detection of MAC address

### Add component to your project

Please use the component manager command `add-dependency` to add the `iot*usbh*ecm` to your project's dependency, during the `CMake` step the component will be downloaded automatically

```
idf.py add-dependency "espressif/iot_usbh_ecm=*"
```

### Docs

* [IOT USBH ECM](https://docs.espressif.com/projects/esp-iot-solution/en/latest/usb/usb*host/usb*ecm.html)

### Examples

* [USB ECM](https://github.com/espressif/esp-iot-solution/tree/master/examples/usb/host/usb*ecm*4g_module)
