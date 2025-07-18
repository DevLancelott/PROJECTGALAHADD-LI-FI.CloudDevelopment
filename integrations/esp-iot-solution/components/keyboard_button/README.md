[![Component Registry](https://components.espressif.com/components/espressif/keyboard*button/badge.svg)](https://components.espressif.com/components/espressif/keyboard*button)

# Component: Keyboard Button
[Online documentation](https://docs.espressif.com/projects/esp-iot-solution/en/latest/input*device/keyboard*button.html)

`keyboard_button` is a library for scanning keyboard matrix, supporting the following features:

List of supported events:
 * KBD*EVENT*PRESSED
 * KBD*EVENT*COMBINATION

* Supports full-key anti-ghosting scanning method.
* Supports efficient key scanning with a scan rate of no less than 1K.
* Supports low-power keyboard scanning.

## Add component to your project

Please use the component manager command `add-dependency` to add the `keyboard_button` to your project's dependency, during the `CMake` step the component will be downloaded automatically

```
idf.py add-dependency "espressif/keyboard_button=*"
```