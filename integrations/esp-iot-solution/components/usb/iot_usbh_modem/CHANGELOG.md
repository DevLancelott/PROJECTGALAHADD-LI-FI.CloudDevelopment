# ChangeLog

## v1.2.0 - 2025-05-20

* update iot*usbh*cdc to 2.0.0

## v1.1.5 - 2024-4-1

* Support `modem*board*deinit()`.

## v1.1.4 - 2024-3-20

* Support 4g module `YM310_X09`
* Add a debug print

## v1.1.3 - 2024-3-20

* Support 4g module `EG25_GL` and `AIR780E`

## v1.1.2 - 2024-3-14

* Avoid `*usb*data*recv*task` from consuming too much time slice.

## v1.1.1 - 2024-3-11

* Add Kconfig MODEM*PRINT*DEVICE_DESCRIPTOR

## v1.1.0 - 2024-11-28

* Support ESP32-P4 with EC20_CE cat.4 module

## v1.0.0 - 2024-11-14

* Use iot*usbh*cdc version 1.0.0

## v0.2.1 - 2023-11-23

* Fix possible cmake_utilities dependency issue

## v0.2.0 - 2023-10-9

* Improve recovery mechanism
* Add new model AIR780E
* Fix concurrency issue in AT command mode
* Fix memory stamp issue in `common*get*operator*after*mode_format`

## v0.1.6 - 2023-04-17

* Fix PPP mode can not exit gracefully
* Fix compatibility issue, remove unnecessary command during startup
* Fix NAPT disable issue (esp-lwip NAPT now have a bug, disable then enable NAPT will cause random table index)

## v0.1.5 - 2023-04-17

* Fix compatibility issue, remove unnecessary command during startup

## v0.1.4 - 2023-03-09

* Set default SoftAP DNS during Wi-Fi config
* Add Ping process in example

## v0.1.3 - 2023-03-08

* Fix NAPT enable issue

## v0.1.2 - 2023-03-06

* Modify the default channel 13 to 6
* Add example link in idf_component.yml

## v0.1.1 - 2023-02-13

* Support IDF5.0
* Change dependency iot*usbh*cdc to private

## v0.1.0 - 2023-02-02

* initial version
