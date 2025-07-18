| Supported Targets | ESP32 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- |

# ESP Camera Picture Server

The example starts a web server on a local network. You can use a browser to access this server to view pictures.    

See the [README.md](../README.md) file in the upper level [camera](../) directory for more information about examples.  

## How to use the example


### Hardware Required

* A development board with camera module (e.g., ESP-EYE, ESP32-S2-Kaluga-1, ESP32-S3-EYE, etc.)
* A USB cable for power supply and programming

### Configure the project

step1: chose your target chip.

````
idf.py menuconfig -> Camera Pin Configuration
````

step2: Configure your wifi.

```
idf.py menuconfig -> Example Configuration
```

step3: Configure the camera.

```
idf.py menuconfig -> component config -> Camera Configuration
```

step 4: Launch and monitor
Flash the program and launch IDF Monitor:

```bash
idf.py flash monitor
```

step 5: Test the example interactively on a web browser (assuming IP is 192.168.43.130):

open path `http://192.168.43.130/pic` to see an HTML web page on the server.

step 6: Click the refresh button to get next picture on the HTML web page.

## Troubleshooting
* If the log shows "gpio: gpio*intr*disable(176): GPIO number error", then you probably need to check the configuration of camera pins, which you can find in the project configuration menu (`idf.py menuconfig`): Component config -> Camera Pin Config.
* If the initialization of the camera sensor fails. Please check the initialization parameters and pin configuration of your camera sensor. 