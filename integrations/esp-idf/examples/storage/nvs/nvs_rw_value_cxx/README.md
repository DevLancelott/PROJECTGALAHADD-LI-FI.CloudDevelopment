| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Non-Volatile Storage (NVS) C++ Read and Write Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to read and write a single integer value using NVS.
It is essentially the same as the nvs*rw*value example. The only difference is that it uses the C++ NVS handle API.
Please see [nvs*rw*value README](../nvs*rw*value/README.md) for more details about this example.

## How to use example

### Hardware required

This example does not require any special hardware, and can be run on any common development board.

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

First run:
```
Opening Non-Volatile Storage (NVS) handle... Done
Reading restart counter from NVS ... The value is not initialized yet!
Updating restart counter in NVS ... Done
Committing updates in NVS ... Done

Restarting in 10 seconds...
Restarting in 9 seconds...
Restarting in 8 seconds...
Restarting in 7 seconds...
Restarting in 6 seconds...
Restarting in 5 seconds...
Restarting in 4 seconds...
Restarting in 3 seconds...
Restarting in 2 seconds...
Restarting in 1 seconds...
Restarting in 0 seconds...
Restarting now.
```

Subsequent runs:

```
Opening Non-Volatile Storage (NVS) handle... Done
Reading restart counter from NVS ... Done
Restart counter = 1
Updating restart counter in NVS ... Done
Committing updates in NVS ... Done

Restarting in 10 seconds...
Restarting in 9 seconds...
Restarting in 8 seconds...
Restarting in 7 seconds...
Restarting in 6 seconds...
Restarting in 5 seconds...
Restarting in 4 seconds...
Restarting in 3 seconds...
Restarting in 2 seconds...
Restarting in 1 seconds...
Restarting in 0 seconds...
Restarting now.
```

Restart counter will increment on each run.

To reset the counter, erase the contents of flash memory using `idf.py erase-flash`, then upload the program again as described above.
