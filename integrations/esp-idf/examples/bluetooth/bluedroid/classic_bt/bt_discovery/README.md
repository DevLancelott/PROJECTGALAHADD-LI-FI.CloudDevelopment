| Supported Targets | ESP32 |
| ----------------- | ----- |

# ESP-IDF BT-DISCOVERY Example

Example of Classic Bluetooth Device and Service Discovery.

This is the example of using APIs to perform inquiry to search for a target device with a Major device type "Phone" or "Audio/Video" in the CoD (Class of Device) field and then performing a search via SDP (Service Discovery Protocol).

## How to use example

### Hardware Required

This example is designed to run on commonly available ESP32 development board, e.g. ESP32-DevKitC.

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

To exit the serial monitor, type `Ctrl-]`.

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

# TUTORIAL

## Includes

```c
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
```

These `includes` are required for the FreeRTOS and underlying system components to run, including the logging functionality and a library to store data in non-volatile flash memory. We are interested in `bt.h`, `esp*bt*main.h`, `esp*bt*device.h` and `esp*gap*bt_api.h`, which expose the Classic Bluetooth APIs required to implement this example.

* `bt.h`: configures the Bluetooth controller and VHCI from the host side.
* `esp*bt*main.h`: initializes and enables the Bluedroid stack.
* `esp*gap*bt_api.h`: implements the GAP configuration, such as Classic Bluetooth device and service discovery.
* `esp*bt*device.h`: implements the device configuration, such as device address and device name.

## Main Entry Point

The program’s entry point is the `app_main()` function.

### Non-volatile Storage Library Initialization

The main function starts by initializing the non-volatile storage library. This library allows to store PHY calibration data and save key-value pairs in flash memory.

```c
esp_err_t ret = nvs_flash_init();
if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
}
ESP_ERROR_CHECK( ret );
```

### Release The Controller Memory

Then the main function releases the controller memory for unused modes. It releases the BLE (Bluetooth Low Energy) memory in the controller via:

```c
ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));
```

The controller memory should be released only before initializing Bluetooth controller or after deinitializing Bluetooth controller.

Note that once Bluetooth controller memory is released, the process cannot be reversed. It means you cannot use the Bluetooth mode which you have released by this function.

## Bluetooth Controller and Stack Initialization

The main function also initializes the Bluetooth controller by first creating the controller configuration structure named `esp*bt*controller*config*t` with default settings generated by the `BT*CONTROLLER*INIT*CONFIG*DEFAULT()` macro. The Bluetooth controller implements the Host Controller Interface (HCI) on the controller side, the Link Layer (LL), and the Physical Layer (PHY). The Bluetooth Controller is invisible to the user applications and deals with the lower layers of the Bluetooth stack. The controller configuration includes setting the Bluetooth controller stack size, priority, and HCI baud rate. With the settings created, the Bluetooth controller is initialized and enabled with the `esp*bt*controller_init()` function:

```c
esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
    ESP_LOGE(GAP_TAG, "%s initialize controller failed: %s", __func__, esp_err_to_name(ret));
    return;
}
```

Next, the controller is enabled in Classic Bluetooth Mode.

```c
if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
    ESP_LOGE(GAP_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
    return;
}
```
> If you want to use the dual mode (BLE + Bluetooth Classic), the controller should be enabled in `ESP*BT*MODE_BTDM`, and should not releases the BLE memory in the controller.

There are four Bluetooth modes supported:

1. `ESP*BT*MODE_IDLE`: Bluetooth not running
2. `ESP*BT*MODE_BLE`: BLE mode
3. `ESP*BT*MODE*CLASSIC*BT`: Bluetooth Classic mode
4. `ESP*BT*MODE_BTDM`: Dual mode (BLE + Bluetooth Classic)

After the initialization of the Bluetooth controller, the Bluedroid stack, which includes the common definitions and APIs for both Bluetooth Classic and BLE, is initialized and enabled by using:

```c
esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
if ((ret = esp_bluedroid_init_with_cfg(&bluedroid_cfg)) != ESP_OK) {
    ESP_LOGE(GAP_TAG, "%s initialize bluedroid failed: %s", __func__, esp_err_to_name(ret));
    return;
}

if ((ret = esp_bluedroid_enable()) != ESP_OK) {
    ESP_LOGE(GAP_TAG, "%s enable bluedroid failed: %s", __func__, esp_err_to_name(ret));
    return;
}
```

The main function ends by starting up application function.

```c
bt_app_gap_start_up();
```

## Application Start Up
The application function starts by registering the GAP (Generic Access Profile) event handlers,

```c
    /* register GAP callback function */
    esp_bt_gap_register_callback(bt_app_gap_cb);
```

The GAP event handlers are the functions used to catch the events generated by the Bluetooth stack and execute functions to configure parameters of the application.

The functions `bt*app*gap_cb` handle all the events generated by the Bluetooth stack.

The application function then sets the device name and sets it as discoverable and connectable. After this, this device can be discovered and connected.

```c
char *dev_name = "ESP_GAP_INQRUIY";
esp_bt_gap_set_device_name(dev_name);

/* set discoverable and connectable mode, wait to be connected */
esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
```

The application function then initialises the information and status of application layer and starts to discover nearby Bluetooth devices.

```c
/* initialize device information and status */
bt_app_gap_init();

/* start to discover nearby Bluetooth devices */
app_gap_cb_t *p_dev = &m_dev_info;
p_dev->state = APP_GAP_STATE_DEVICE_DISCOVERING;
esp_bt_gap_start_discovery(ESP_BT_INQ_MODE_GENERAL_INQUIRY, 10, 0);
```

## GAP Callback

### Get Device Discovery Result

After device discovery gets started the state will change to `ESP*BT*GAP*DISCOVERY*STARTED`, and `bt*app*gap*cb` will be called with `ESP*BT*GAP*DISC*STATE*CHANGED_EVT`.

If nearby Bluetooth devices are discovered `bt*app*gap*cb` will be called with `ESP*BT*GAP*DISC*RES*EVT`, to post the result to users.

```c
case ESP_BT_GAP_DISC_RES_EVT: {
    update_device_info(param);
    break;
}
```

### Search For A Target Device

Function `update*device*info` gets the CoD (Class of Device), RSSI (Received Signal Strength Indication), name, EIR (Extended Inquiry Result) from the discovery result.

```c
for (int i = 0; i < param->disc_res.num_prop; i++) {
    p = param->disc_res.prop + i;
    switch (p->type) {
    case ESP_BT_GAP_DEV_PROP_COD:
        cod = *(uint32_t *)(p->val);
        ESP_LOGI(GAP_TAG, "--Class of Device: 0x%x", cod);
        break;
    case ESP_BT_GAP_DEV_PROP_RSSI:
        rssi = *(int8_t *)(p->val);
        ESP_LOGI(GAP_TAG, "--RSSI: %d", rssi);
        break;
    case ESP_BT_GAP_DEV_PROP_BDNAME:
        bdname_len = (p->len > ESP_BT_GAP_MAX_BDNAME_LEN) ? ESP_BT_GAP_MAX_BDNAME_LEN :
                      (uint8_t)p->len;
        bdname = (uint8_t *)(p->val);
        break;
    case ESP_BT_GAP_DEV_PROP_EIR: {
        eir_len = p->len;
        eir = (uint8_t *)(p->val);
        break;
    }
    default:
        break;
    }
}
```

Pay attention that some Bluetooth devices may put their name in EIR data. We can get the device name from EIR data.

```c
if (p_dev->bdname_len == 0) {
    get_name_from_eir(p_dev->eir, p_dev->bdname, &p_dev->bdname_len);
}
```

Then, function `update*device*info` searches for a device with Major device type "Phone" or "Audio/Video" according to COD. For more information on COD, check [Bluetooth specifications](https://www.bluetooth.com/specifications/assigned-numbers/).

```c
if (!esp_bt_gap_is_valid_cod(cod) ||
    (!(esp_bt_gap_get_cod_major_dev(cod) == ESP_BT_COD_MAJOR_DEV_PHONE) &&
         !(esp_bt_gap_get_cod_major_dev(cod) == ESP_BT_COD_MAJOR_DEV_AV))) {
    return;
}
```

If the target device is found, the device discovery procedure is stopped.

```c
ESP_LOGI(GAP_TAG, "Found a target device, address %s, name %s", bda_str, p_dev->bdname);
p_dev->state = APP_GAP_STATE_DEVICE_DISCOVER_COMPLETE;
ESP_LOGI(GAP_TAG, "Cancel device discovery ...");
esp_bt_gap_cancel_discovery();
```

### Service Discovery

After device discovery stops the state will change to `ESP*BT*GAP*DISCOVERY*STOPPED` and `bt*app*gap*cb` will be called with `ESP*BT*GAP*DISC*STATE*CHANGED_EVT`.

Then it performs service discovery targeting at this device.

```c
p_dev->state = APP_GAP_STATE_SERVICE_DISCOVERING;
ESP_LOGI(GAP_TAG, "Discover services ...");
esp_bt_gap_get_remote_services(p_dev->bda);
```

Then `bt*app*gap*cb` will be called with `ESP*BT*GAP*RMT*SRVCS*EVT` to post the service discovery result to users including the service UUID. For more information on the service UUID, you can check [Bluetooth specifications](https://www.bluetooth.com/specifications/assigned-numbers/).

```c
for (int i = 0; i < param->rmt_srvcs.num_uuids; i++) {
    esp_bt_uuid_t *u = param->rmt_srvcs.uuid_list + i;
    ESP_LOGI(GAP_TAG, "--%s", uuid2str(u, uuid_str, 37));
}
```

## Conclusion

We have reviewed the Bluetooth discovery example code. This example searches for nearby devices. When the device of interest is found, the example searches for the services of this device. We can get the device discovery result and service discovery result from the GAP callback, which is registered in advance.
